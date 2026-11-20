#include <iostream>
#include <nlohmann/json.hpp>
#include <cpr/cpr.h>            //Как раз для http запросов
#include <format>
#include <thread>               // Для задержки
#include <chrono>
#include <filesystem>
#include <fstream>
#include "lib/StorageStructures.hpp"     // Структура для descriptor
#include "parsejson.hpp"
#include "webparser.hpp"
 

using json = nlohmann::json;
namespace fs = std::filesystem;

// ====================================


/// @breif Parser init
int parserInit(cpr::Session& session, const std::string& mangalib_session = "", const std::string& xsrf_token = "") {

    cpr::Cookies cookies = {
        {"mangalib_session", mangalib_session.empty() ? "" : mangalib_session},
        {"XSRF-TOKEN", xsrf_token.empty() ? "" : xsrf_token}
    };

    cpr::Header headers = {
        {"Host", "api.imglib.info"},
        {"User-Agent", "Mozilla/5.0 (X11; Linux x86_64; rv:154.0) Gecko/20100101 Firefox/154.0"},
        {"Accept-Language", "en-US,en;q=0.9"},
        {"Accept-Encoding", "gzip, deflate, br, zstd"},
        {"Referer", "https://ranobelib.me/"},
        {"Origin", "https://ranobelib.me"},
        {"Content-Type", "application/json"},
        {"Site-Id", "3"}
    };

    session.SetHeader(headers);
    session.SetCookies(cookies);

    return 1;
}


#if 0
 /// @breif Функция для сохранения Cooki
 void saveCookies(const cpr::Cookies& cookies, const std::string& filename) {
     json j = json::object();
 
     for(const auto& [name, value] : cookies) {
         j[name] = value;
     }
 
     std::ofstream file(filename);
     if(!file.is_open()){
         std::cerr << "Не удалось открыть файл для записи: " << filename << std::endl;
         return;
     }
 
     file << j.dump(2);
     std::cout << "Save Cookie in file: " << filename << std::endl;
 }

void loadCookies() { }
#endif

/// std::this_thread::sleep_for(std::chrono::seconds(2)); //Сделать задержку на 2 секунды

std::string get_slug_url_from_url(std::string url);

/// @brief Test function dla proverki otpravki zaprosov
void get_req(cpr::Session* session, const std::string& url){
    session->SetUrl(url);
    cpr::Response resp = session->Get();
   
    if(resp.status_code == 200){
        std::cerr << "Resp:\n" << resp.text << std::endl;

    } else {
        std::cerr << "Pars error: " << resp.status_code << std::endl;
//        std::cerr << "Error text:\n" << resp.text << std::endl;
    }


}

/*
 * @brief Функция для загрузки/выгрузки кэша
 * Работа с кешем. Функция для отлаживания работы с json, чтобы не отправлять запросы к сайту
 */
json cash_prc(const std::string& fname, const json& load_data = json{}) {

    fs::path cash_dir = "tmp/cash";
    fs::create_directories(cash_dir);
    fs::path file_path = cash_dir / fname;

    if (load_data.empty()) {
        // Чтение из кеша
        std::ifstream file(file_path);
        if (!file.is_open()) {
            std::cerr << "Не удалось открыть кеш-файл для чтения: " << fname << std::endl;
            return json{};
        }
        json out_json;
        file >> out_json;
        std::cout << "Кэш из файла " << fname << " экспортирован" << std::endl;
        return out_json;

    } else {
        // Запись в кеш
        std::ofstream file(file_path);
        if (!file.is_open()) {
            std::cerr << "При создании кеш файла, не удалось открыть файл: " << fname << std::endl;
            return load_data;
        }
        file << load_data.dump(4);
        std::cout << "Кэш в файл " << fname << " импортирован" << std::endl;
        return load_data;
    }
}

/// TODO: Реализовать динамическую задержку!

/// @breif Получение сырых данных
std::vector<json> get_data(cpr::Session* session, ModBookInfo* bookDescr){
    int chapter_num = 60;
    int volume_num = 1;

    std::string cash_titl_info_fname = "title_info_cach_fname.json";
    std::string cash_chapter_info_fname = std::format("chapter_data_cash-V{}Ch{}.json", volume_num, chapter_num);
   
    json chapter_raw_json = cash_prc(cash_chapter_info_fname);
    json title_raw_json = cash_prc(cash_titl_info_fname);

    if(chapter_raw_json.empty()){
        json rq2 = get_chapter_content(session, bookDescr->slug_url, volume_num, chapter_num);
        chapter_raw_json = cash_prc(cash_chapter_info_fname, rq2);
    }

    if(title_raw_json.empty()){
        json rq1 = get_title_info(session, bookDescr->slug_url);
        title_raw_json = cash_prc(cash_titl_info_fname, rq1);
    }
    
    std::vector<json> outd = {chapter_raw_json, title_raw_json};

    return outd;
}


//struct ChapterData {
//    int volume;
//    int chapter;
//    int branch_id;
//    std::string translate_teem;
//};

//struct ModBookInfo : LibBook {
//    std::string slug_url;
//}; 

void test(std::string url){

    ModBookInfo bookDescr {.slug_url=get_slug_url_from_url(url)};

    cpr::Session session;

    parserInit(session);

    // ===== тестирование дескриптора
    std::vector<json> test_data = get_data(&session, &bookDescr);
    prc_bookInfo_from_json(test_data[1], &bookDescr);

    ChapterData chapterData = prc_one_chapters_from_json(test_data[0]);

    std::cout <<
        "\n============ TEST BookDescriptor ==============\n"
        "languages: " << bookDescr.languages[0] << "\n"
        "title: " << bookDescr.title << "\n"
        "original title: " << bookDescr.original_title << "\n"
        "slug_url: "<< bookDescr.slug_url << "\n"
        "Country of origin of the book: " << bookDescr.country_of_origin_of_the_book << "\n"
        "Description: \n--------------\n" << bookDescr.description << "\n--------------\n"
        << std::endl;

    std::cout << "Tags:" << std::endl;
    std::string tags = "";
    for (std::string tag : bookDescr.tags){
        tags += tag + "; ";
    }
    std::cout << tags << std::endl;

    std::cout << "Genes:" << std::endl;
    std::string genres = "";
    for (std::string genre : bookDescr.genres){
        genres += genre + "; ";
    }
    std::cout << genres << std::endl;

    // --------
    
    std::cout <<
        "\n========== TEST Chapter Data ===========\n"
        "Chapter name: " << chapterData.name << "\n"
        "Volume num: " << chapterData.volume << "\n"
        "Chapter num: " << chapterData.chapter << "\n"
        "Branch_id: " << chapterData.branch_id << "\n"
        "Create at: " << chapterData.created_at << "\n"
        "Content:\n" << convert_vector_docBlock_to_text(chapterData.raw_content)
    << std::endl;

}

int main(){

    std::string url = "https://ranobelib.me/ru/book/203736--seoul-object-story";
    fs::path tmp_dir = "tmp";

    test(url);

    // =====
    return 0;
}

/// @breif Get slug url fron url string
std::string get_slug_url_from_url(std::string url){
    std::string find_substr = "ranobelib.me/ru/book/";
    size_t pos = url.find(find_substr);

    if (pos != std::string::npos) {
        std::string slug_url = url.substr(pos + find_substr.length());
        return slug_url;
    } else {
        std::cerr << "Url error" << std::endl;
        return "";
    }
}



// =================== JSON


/**
 [summary][content][0...][type | content]
         "summary": {
            "content": [
                {
                    "content": [
                        {
                            "text": "Человечество",
                            "type": "text"
                        }
                    ],
                    "type": "paragraph"
                },
                {
                    "content": [
                        {
                            "text": "Эта история о реинкарнации в Объект и жизнь без забот в Сеуле такого мира.",
                            "type": "text"
                        }
                    ],
                    "type": "paragraph"
                }
            ]
*/ 
