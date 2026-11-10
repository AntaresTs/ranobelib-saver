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

// URL = https://ranobelib.me/ru/book/203736--seoul-object-story

struct ChapterData {
    int volume;
    int chapter;
    int branch_id;
    std::string id_url_title = "";
    std::string translate_teem;
};

// Инициилизация парсера
void parserInit(cpr::Session* session, const cpr::Cookies& cookies);

// /// @breif Функция для сохранения Cooki
// void saveCookies(const cpr::Cookies& cookies, const std::string& filename) {
//     json j = json::object();
// 
//     for(const auto& [name, value] : cookies) {
//         j[name] = value;
//     }
// 
//     std::ofstream file(filename);
//     if(!file.is_open()){
//         std::cerr << "Не удалось открыть файл для записи: " << filename << std::endl;
//         return;
//     }
// 
//     file << j.dump(2);
//     std::cout << "Save Cookie in file: " << filename << std::endl;
// }

//void loadCookies() {
//    return;
//}


/// @brief отправка запроса и получение json
json get_json_from_url(cpr::Session* session, const std::string& url){
    session->SetUrl(url);
    cpr::Response resp = session->Get();
   
    if(resp.status_code == 200){
        return json::parse(resp.text);

    } else {
        std::cerr << "Pars error: " << resp.status_code << std::endl;
        return json::object();
    }

}

/// @brief Получить контент главы в виде json
/// @paran id_url_title str - Id главы, используемый в ссылке
json get_chapter_content(cpr::Session* session, const std::string& slug_url, const int& volume, const int& chapter, const std::string& branch_id = ""){
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
LibBook prc_bookInfo_from_json(cpr::Session* session, const std::string& slug_url, json json_title_info = nullptr){
//    json json_chapter_content = get_chapter_content(&session, chapterData.id_url_title, chapterData.volume, chapterData.chapter, chapterData.branch_id);
    if(json_title_info.is_null()){
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

int main(){
    std::string url = "https://ranobelib.me/ru/book/203736--seoul-object-story";
    std::string slug_url = get_slug_url_from_url(url);
    fs::path tmp_dir = "tmp";
    

//    ChapterData chapterData{1, 224, 22612, "203736--seoul-object-story", ""};
//    LibBook bookInfo;
//
//    cpr::Url test_url = {"http://192.168.0.18:8000"};

    cpr::Session session;
    cpr::Cookies cookies = {
        {"mangalib_session", ""},
        {"XSRF-TOKEN", ""}
    };

    parserInit(&session, cookies);

    std::cout << "===== Тестовая отправка запроса ====" << std::endl;
    json rq1 = get_title_info(&session, slug_url);
    json rq2 = get_chapter_content(&session, slug_url, 1, 60);

    std::cout << "==== Получение данных прошло успешно ====" << std::endl;

    fs::create_directory(tmp_dir);
    
    //std::ofstream f_title_info(tmp_dir + "/title_info.json");
    //std::ofstream f_chapter_info(tmp_dir + "/chapter_info.json");

    std::ofstream f_test(tmp_dir / "f_test.json");
    
    if (f_test.is_open()) {
        f_test << rq1.dump(4) << std::endl;
        f_test << "\n====================\n" << std::endl;
        f_test << rq1.dump(4) << std::endl;
    }




    //

    // ===== Окончательная сборка парсера

    // std::cout << "Status: " << resp1.status_code << std::endl;
    // std::cout << json::parse(resp1.text).dump(2) << std::endl;

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
        {"User-Agent", "Mozilla/5.0 (X11; Linux x86_64; rv:154.0) Gecko/20100101 Firefox/154.0"},
        {"Origin", "https://ranobelib.me"},
        {"Content-Type", "application/json"},
        {"Host", "api.imglib.info"},
        {"Accept-Language", "en-US,en;q=0.9"},
        {"Accept-Encoding", "gzip, deflate, br, zstd"}
    };

    session->SetHeader(headers);
    session->SetCookies(cookies);
}
