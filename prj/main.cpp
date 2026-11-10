#include <iostream>
#include <nlohmann/json.hpp>
#include <cpr/cpr.h>            //Как раз для http запросов
#include <format>
#include <thread>               // Для задержки
#include <chrono>
#include <filesystem>
#include <fstream>
#include "lib/StorageStructures.hpp"     // Структура для descriptor

using json = nlohmann::json;
namespace fs = std::filesystem;



struct ChapterData {
    int volume;
    int chapter;
    int branch_id;
    std::string translate_teem;
};

// Инициилизация парсера
void parserInit(cpr::Session* session, const cpr::Cookies& cookies);

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

void loadCookies() {
    return;
}
#endif


/// @brief отправка запроса и получение json
json get_json_from_url(cpr::Session* session, const std::string& url){
    session->SetUrl(url);
    cpr::Response resp = session->Get();
   
    if(resp.status_code == 200){
        return json::parse(resp.text);

    } else {
        std::cerr << "Pars error: " << resp.status_code << std::endl;
        std::cerr << "Error text:\n" << resp.text << std::endl;
        return json::object();
    }

}

/// @brief Получить контент главы в виде json
/// @paran id_url_title str - Id главы, используемый в ссылке
json get_chapter_content(cpr::Session* session, const std::string& slug_url, const int& volume, const int& chapter){
    //std::string url = std::format("https://api.imglib.info/api/manga/{}/chapter?volume={}&number={}&branch_id={}", slug_url, volume, chapter, branch_id);
    std::string url = std::format("https://api.imglib.info/api/manga/{}/chapter?volume={}&number={}", slug_url, volume, chapter);
    return get_json_from_url(session, url);
}

// Получение информации о книге
json get_title_info(cpr::Session* session, const std::string& slug_url) {
    // 203736--seoul-object-story

    std::string url = std::format(
        "https://api.imglib.info/api/manga/{}?fields[0]=background&fields[1]=eng_name&fields[2]=otherNames&fields[3]=summary&fields[4]=releaseDate&fields[5]=country&fields[6]=type_id&fields[7]=caution&fields[8]=views&fields[9]=close_view&fields[10]=rate_avg&fields[11]=rate&fields[12]=genres&fields[13]=tags&fields[14]=teams&fields[15]=user&fields[16]=franchise&fields[17]=authors&fields[18]=publisher&fields[19]=userRating&fields[20]=moderated&fields[21]=metadata&fields[22]=metadata.count&fields[23]=metadata.close_comments&fields[24]=translation_quality_rating&fields[25]=manga_status_id&fields[26]=chap_count&fields[27]=status_id&fields[28]=extra_release_status&fields[29]=artists&fields[30]=format",
        slug_url
    );
    return get_json_from_url(session, url);
}

/// @brief Функция для удобства
std::vector<std::string> extract_strings_from_json_object(const json& j, const std::string& field, const std::string& subfield){
    std::vector<std::string> out_data;
    if (j.contains(field) && j[field].is_object()){
        for (const auto& [_, value] : j.at(field).items()){
            out_data.push_back(value.value(subfield, std::string{}));
        }
    }

    return out_data;
}

/// @brief распарсинг json. Запись в структуру информации о книге
LibBook prc_bookInfo_from_json(cpr::Session* session, const std::string& slug_url, json json_title_info = json{}){
//    json json_chapter_content = get_chapter_content(&session, chapterData.id_url_title, chapterData.volume, chapterData.chapter, chapterData.branch_id);
    if(json_title_info.empty()){
        json_title_info = get_title_info(session, slug_url);
    }

    LibBook bookDescriptor;

    bookDescriptor.languages.push_back("ru");

    // ===========

    json j = json_title_info.at("data");

    // Оригинальное названеи произведения
    bookDescriptor.original_title = j.at("name").get<std::string>();

    // Названеи произведения на русском
    bookDescriptor.title = j.at("rus_name").get<std::string>();

    // Альтернативные названия
    bookDescriptor.alternative_titles = j.at("otherNames").get<std::vector<std::string>>();

    // тут несколько типов обложек
    //json cover = j.at("cover");
    
    // Возрастное ограничение, в формате текста
    std::string ageRestriction = j.contains("ageRestriction") ? j["ageRestriction"].at("label").get<std::string>() : std::string{}; //j.at("ageRestriction").value("label", std::string{});
    
    // Страна происхождения
    bookDescriptor.country_of_origin_of_the_book = j.contains("type") && j["type"].is_object() ? j["type"].at("label").get<std::string>() : std::string{}; //j.at("type").value("label", std::string{});
    
    // Содержит словарь, с ключами от 0 и т.д., номера параграфов. Словарь содержит
    // type - "text"
    // text - содержание параграфа
    // TODO: !
    json summary = j.at("content");

    // Дата публикации
    bookDescriptor.year_of_publication = j.at("releaseDate").get<std::string>();
    
    // Команды превода
    // Содержит словарь, с ключами от 0 и т.д.. 
    // имеет поле name с ником автора
    std::vector<std::string> translate_teems = extract_strings_from_json_object(j, "teams", "name");


    // жанры
    // Словарь с ключами от 0... 
    // Есть поле name с названием жанра
    bookDescriptor.genres = extract_strings_from_json_object(j, "genres", "name");
    
    // То же самое что и с полем с жанрами, но с тегами
    bookDescriptor.tags = extract_strings_from_json_object(j, "tags", "name");
    
    // Издатели
    bookDescriptor.publisher = {j.contains("publisher") && j["publisher"].is_object() ? j["publisher"].at("name").get<std::string>() : std::string{}};

    // Франшиза. Или же пародия
    std::vector<std::string> franchise = {j.contains("franchise") ? j["franchise"].at("name").get<std::string>() : std::string{}};

    // Список авторов. тоже стандартный словарь. Нужно парсить поле .{num}.name
    bookDescriptor.authors = extract_strings_from_json_object(j, "authors", "name");

    // Список художников. тоже стандартный словарь. .{num}.name
    bookDescriptor.artists = extract_strings_from_json_object(j, "artists", "name");

    return bookDescriptor;
}


/// std::this_thread::sleep_for(std::chrono::seconds(2)); //Сделать задержку на 2 секунды
// ==========

std::string get_slug_url_from_url(std::string url);


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

json cash_prc(const std::string& fname, const json& load_data = json{}) {
    /// @brief Работа с кешем. Функция для отлаживания работы с json, чтобы не отправлять запросы к сайту

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

/// @brief пример парсинга данных и записи их в кэш.
void exemple_1(cpr::Session* session, const std::string& slug_url){
    ChapterData test_cd (1, 60, 0, ""); 
    json rq1 = get_title_info(session, slug_url);
    json rq2 = get_chapter_content(session, slug_url, 1, 60);

    if(rq1.empty() ||  rq2.empty()) {
        std::cout << "==== При получении данных произошла ошибка ====" << std::endl;
        return 1;
    } else {
        std::cout << "==== Данные успешно получены ====" << std::endl;
        std::string chapter_info_cach_fname = std::format("chapter_data_cash-V{}Ch{}.json", test_cd.volume, test_cd.chapter);

        cash_prc("title_info_cach_fname.json", rq1);
        cash_prc(chapter_info_cach_fname, rq2);

        std::cout << "Данные закешированны" << std::endl;

        // Демонстрация получения данных из кэша
        std::cout << cash_prc(chapter_info_cach_fname) << std::endl;
    }
}


//struct ChapterData {
//    int volume;
//    int chapter;
//    int branch_id;
//    std::string translate_teem;
//};

int main(){

    std::string url = "https://ranobelib.me/ru/book/203736--seoul-object-story";
    std::string slug_url = get_slug_url_from_url(url);
    fs::path tmp_dir = "tmp";

//    LibBook bookInfo;
//
//    cpr::Url test_url = {"http://192.168.0.18:8000"};

    cpr::Session session;
    cpr::Cookies cookies = {
        {"mangalib_session", ""},
        {"XSRF-TOKEN", ""}
    };

    parserInit(&session, cookies);
    
    return 0;
}

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

void parserInit(cpr::Session* session, const cpr::Cookies& cookies) {

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

    session->SetHeader(headers);
    session->SetCookies(cookies);
}
