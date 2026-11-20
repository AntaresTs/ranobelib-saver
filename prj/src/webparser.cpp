#include <iostream>
#include <cpr/cpr.h>            //Как раз для http запросов
#include <nlohmann/json.hpp>
#include <fstream>

using json = nlohmann::json;
namespace fs = std::filesystem;



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

/*
 * @brief Получить контент главы в виде json
 * @paran id_url_title str - Id главы, используемый в ссылке
 */
json get_chapter_content(cpr::Session* session, const std::string& slug_url, const int& volume, const int& chapter){
    //std::string url = std::format("https://api.imglib.info/api/manga/{}/chapter?volume={}&number={}&branch_id={}", slug_url, volume, chapter, branch_id);
    std::string url = std::format("https://api.imglib.info/api/manga/{}/chapter?volume={}&number={}", slug_url, volume, chapter);
    return get_json_from_url(session, url);
}

// @breif Получение информации о книге
json get_title_info(cpr::Session* session, const std::string& slug_url) {
    std::string url = std::format(
        "https://api.imglib.info/api/manga/{}?fields[0]=background&fields[1]=eng_name&fields[2]=otherNames&fields[3]=summary&fields[4]=releaseDate&fields[5]=country&fields[6]=type_id&fields[7]=caution&fields[8]=views&fields[9]=close_view&fields[10]=rate_avg&fields[11]=rate&fields[12]=genres&fields[13]=tags&fields[14]=teams&fields[15]=user&fields[16]=franchise&fields[17]=authors&fields[18]=publisher&fields[19]=userRating&fields[20]=moderated&fields[21]=metadata&fields[22]=metadata.count&fields[23]=metadata.close_comments&fields[24]=translation_quality_rating&fields[25]=manga_status_id&fields[26]=chap_count&fields[27]=status_id&fields[28]=extra_release_status&fields[29]=artists&fields[30]=format",
        slug_url
    );
    return get_json_from_url(session, url);
}
