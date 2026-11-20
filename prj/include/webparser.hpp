#pragma once

# if 0
#include <nlohmann/json.hpp>
#include <cpr/cpr.h>            //Как раз для http запросов
# endif

/// @breif Parser init
//cpr::Session parserInit(cpr::Session* session, const std::string& mangalib_session = "", const std::string& xsrf_token = "");

/*
 * @brief Получить контент главы в виде json
 * @paran id_url_title str - Id главы, используемый в ссылке
 */
json get_chapter_content(cpr::Session* session, const std::string& slug_url, const int& volume, const int& chapter);

// @breif Получение информации о книге
json get_title_info(cpr::Session* session, const std::string& slug_url);
