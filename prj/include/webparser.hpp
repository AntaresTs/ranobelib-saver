#pragma once

/*
 * @brief Получить контент главы в виде json
 * @paran id_url_title str - Id главы, используемый в ссылке
 */
json get_chapter_content(cpr::Session* session, const std::string& slug_url, const int& volume, const int& chapter);

// @breif Получение информации о книге
json get_title_info(cpr::Session* session, const std::string& slug_url);
