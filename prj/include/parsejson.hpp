#pragma once

#include <iostream>
#include <vector>
#include <nlohmann/json.hpp>
#include "lib/StorageStructures.hpp"     // Структура для descriptor

using json = nlohmann::json;

/**
 * Parser function:
 * Json -> parsFunction() -> struct
 */

/**
 * @brief Один элемент разобранного документа.
 *
 * Хранит "плоское" представление узла: тип (например, "paragraph" или
 * "horizontalRule") и уже склеенный текст. Для картинок в будущем сюда
 * можно добавить поле `url`/`path` и т.п.
 */
struct DocBlock {
    std::string type;       ///< Тип блока: "paragraph", "horizontalRule", ...
    std::string text;       ///< Склеенный текст (для paragraph); для hr — "---".
    std::string image_url;
};

/**
 * @brief хранит данные о конкретной главе
 */
struct ChapterData {
    int volume;
    int chapter;
    int branch_id;
    std::vector<std::string> translate_teems;
    std::vector<DocBlock> raw_content;          ///< Suda budet pomeshtsa soderjimoye chapter in raw format
};

/**
 * @brief Модификация book description из библиотеки
 */
struct ModBookInfo : LibBook {
    std::string slug_url;
    std::vector<ChapterData> chapters;
}; 

/// @brief распарсинг json. Запись в структуру информации о книге
void prc_bookInfo_from_json(const json& json_title_info, ModBookInfo* bookDescriptor);

