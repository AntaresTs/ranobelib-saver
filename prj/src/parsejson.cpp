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
    std::string name;
    int volume;
    int chapter;
    int branch_id;
    std::string created_at;
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


/**
 * @brief (AI) Рекурсивно извлекает текст из узла документного дерева.
 *
 * Проходит по массиву `content` текущего узла и склеивает все значения поля
 * `text` вложенных узлов (например, `paragraph` -> `text`). Если узел не
 * имеет `content` или `text`, рекурсия просто не даёт вклада.
 *
 * @param node JSON-узел, из которого нужно извлечь текст.
 * @return Склеенная строка текста (может быть пустой для не-текстовых узлов).
 */
static std::string extract_text(const json& node) {
    std::string out;

    // Если у узла есть собственный текст — добавляем его.
    if (node.contains("text") && node["text"].is_string()) {
        out += node["text"].get<std::string>();
    }

    // Рекурсивно обходим вложенные узлы (например, paragraph -> text).
    if (node.contains("content") && node["content"].is_array()) {
        for (const auto& child : node["content"]) {
            out += extract_text(child);
        }
    }
    return out;
}

/**
 * @brief (AI) Разбирает массив узлов `content` в вектор блоков {@link DocBlock}.
 *
 * Ожидает на входе уже извлечённый массив `content` документа, например:
 * @code{.cpp}
 * std::vector<json> summary =
 *     j.contains("summary") && j["summary"].contains("content")
 *         ? j["summary"]["content"].get<std::vector<json>>()
 *         : std::vector<json>{};
 * @endcode
 *
 * Поддерживаются типы:
 *  - `paragraph` — текст склеивается через {@link extract_text}, результат
 *    кладётся в {@link DocBlock::text}; блок добавляется в результат, даже
 *    если текст получился пустым (это может быть намеренный пустой абзац).
 *  - `horizontalRule` — соответствует `---` в Markdown / `<hr>` в HTML;
 *    создаётся блок с пустым текстом.
 *
 * Прочие типы (например, `image`) сейчас пропускаются. Чтобы их поддержать,
 * достаточно добавить ветку `if/else if` и, при необходимости, расширить
 * {@link DocBlock} дополнительными полями (например, `url`).
 *
 * Порядок блоков в результате совпадает с порядком элементов во входном
 * массиве, что важно для правильного позиционирования картинок и
 * разделителей относительно текста.
 *
 * @param content Массив узлов `content` из документа (может быть пустым).
 * @return Вектор разобранных блоков в порядке следования в документе.
 */
std::vector<DocBlock> parse_doc_blocks(const std::vector<json>& content) {
    std::vector<DocBlock> blocks;
    blocks.reserve(content.size());

    for (const auto& node : content) {
        // Пропускаем некорректные узлы: не объект или без поля "type".
        if (!node.is_object() || !node.contains("type") || !node["type"].is_string()) {
            continue;
        }
        const std::string type = node["type"].get<std::string>();

        DocBlock b;
        b.type = type;

        if (type == "paragraph") {
            b.text = extract_text(node);
            blocks.push_back(std::move(b));
        } else if (type == "horizontalRule") {
            b.text = "---";
            blocks.push_back(std::move(b));
        } else {
            // TODO: image и прочие типы — добавить при необходимости.
        }
    }

    return blocks;
}

/**
 * @brief (AI*) Разбирает один узел `content` в DocBlock.
 *
 * Бросает исключение, если тип не поддерживается — чтобы `get<vector<DocBlock>>()`
 * не молча терял данные. Если хотите «пропускать» неподдерживаемые узлы,
 * используйте вариант `parse_doc_blocks` ниже.
 */
DocBlock parse_one_block(const json& node) {
    // Пропускаем некорректные узлы: не объект или без поля "type".
    if (!node.is_object() || !node.contains("type") || !node["type"].is_string()) {
        throw std::invalid_argument("parse_one_block: node without string 'type'");
    }
    const std::string type = node["type"].get<std::string>();

    DocBlock b;
    b.type = type;

    if (type == "paragraph") {
        b.text = extract_text(node);
    } else if (type == "horizontalRule") {
        b.text = "---";
    } else {
        // TODO: image и прочие типы — добавить при необходимости.
        throw std::invalid_argument("parse_one_block: неподдерживаемый тип: " + type);
    }

    return b;
}



/**
 * @brief Функция для удобства
* извекает из json строки и помещает их в вектор
*/ 
std::vector<std::string> extract_strings_from_json_object(const json& j, const std::string& field, const std::string& subfield){
    std::vector<std::string> out_data;
    if (j.contains(field)){ // && j[field].is_object()
        for (const auto& [_, value] : j.at(field).items()){
            out_data.push_back(value.value(subfield, std::string{}));
        }
    }

    return out_data;
}

/**
 * @brief Function for converting a vector<DocBlock> into a string.
 * May not be suitable for the content of the chapter
 */
std::string convert_vector_docBlock_to_text(const std::vector<DocBlock>& raw_content){
    std::string content = "";
    
    for(const auto& b : raw_content){
        content += b.text + "\n\n";
    }

    return content;
}

/**
 * @brief функция для конвертации из json в нужную структуру
 */
void from_json(const json& j, DocBlock& b){
    b = parse_one_block(j);
}

/**
 * @brief функция для конвертации из структуры в json
 */
void to_json(json& j, const DocBlock& b) {
    j = json{{"type", b.type}, {"text", b.text}};
}
/*
 * ==================================
 * Dalee parsing json
 * ==================================
 */

/// @brief распарсинг json. Запись в структуру информации о книге
void prc_bookInfo_from_json(const json& json_title_info, ModBookInfo* bookDescriptor){

    json j = json_title_info.at("data");

    // Пока сойдет и так
    bookDescriptor->languages.push_back("ru");

    /// ==== Что имеет поле в book descriptor ==== 

    // Оригинальное названеи произведения
    bookDescriptor->original_title = j.at("name").get<std::string>();

    // Названеи произведения на русском
    bookDescriptor->title = j.contains("rus_name") ? j.at("rus_name").get<std::string>() : std::string{};

    // Альтернативные названия
    bookDescriptor->alternative_titles = j.contains("otherNames") ? j.at("otherNames").get<std::vector<std::string>>() : std::vector<std::string>{};
    std::cout << "Alt name: " << bookDescriptor->alternative_titles[0] << std::endl;

    // Страна происхождения
    bookDescriptor->country_of_origin_of_the_book = j.contains("type")? j["type"].at("label").get<std::string>() : std::string{}; //j.at("type").value("label", std::string{});

    // Дата публикации
    bookDescriptor->year_of_publication = j.contains("releaseDate") ? j.at("releaseDate").get<std::string>() : std::string{};

    // жанры
    // Словарь с ключами от 0... 
    // Есть поле name с названием жанра
    bookDescriptor->genres = extract_strings_from_json_object(j, "genres", "name");
    
    // То же самое что и с полем с жанрами, но с тегами
    bookDescriptor->tags = extract_strings_from_json_object(j, "tags", "name");
    
    // Издатели
    std::vector<json> json_publishers = j.contains("publisher") ? j["publisher"].get<std::vector<json>>() : std::vector<json>{};
    
    for (json publisher : json_publishers){
       bookDescriptor->publishers.push_back(publisher["name"]);
    }

    //bookDescriptor->publisher = j.contains("publisher") ? j["publisher"].at("name").get<std::string>() : std::string{};

    // Список авторов. тоже стандартный словарь. Нужно парсить поле .{num}.name
    bookDescriptor->authors = extract_strings_from_json_object(j, "authors", "name");

    // Список художников. тоже стандартный словарь. .{num}.name
    bookDescriptor->artists = extract_strings_from_json_object(j, "artists", "name");

    // Содержит словарь, с ключами от 0 и т.д., номера параграфов. Словарь содержит
    std::vector<DocBlock> summary = j.contains("summary") && j["summary"].contains("content") ? j["summary"]["content"].get<std::vector<DocBlock>>() : std::vector<DocBlock>{};
    bookDescriptor->description = convert_vector_docBlock_to_text(summary);

    /// ==== Что не имеет поле в book descriptor ===
    
    // тут несколько типов обложек
    //json cover = j.at("cover");

    // Возрастное ограничение, в формате текста
    //std::string ageRestriction = j.contains("ageRestriction") ? j["ageRestriction"].at("label").get<std::string>() : std::string{};

    /* Команды превода
     * Содержит словарь, с ключами от 0 и т.д.. 
     * имеет поле name с ником переводчика.
     * По сути бесполезен, т.к. кто перевевел, это свойство главы
     */
    //std::vector<std::string> translate_teems = extract_strings_from_json_object(j, "teams", "name");

    // Франшиза. Или же пародия
    //std::vector<std::string> franchise = {j.contains("franchise") ? j["franchise"].at("name").get<std::string>() : std::string{}};

}


/* @breif Functions for fulling chapter structure
 * Tut proishodit razbor json dannyh dla 1 chapter. 
 */
ChapterData prc_one_chapters_from_json(json json_chapter_info){
    ChapterData chapterDat;

    json j = json_chapter_info.at("data");

    /*    
    struct ChapterData {
        std::string name;
        int volume;
        int chapter;
        int branch_id;
        std::string created_at;
        std::vector<std::string> translate_teems;
        std::vector<DocBlock> raw_content;          ///< Suda budet pomeshtsa soderjimoye chapter in raw format
    };
    */

    chapterDat.branch_id = j.at("branch_id").get<std::string>();
    chapterDat.created_at = j.at("created_at").get<std::string>();
    chapterDat.name = j.at("name").get<std::string>();
    chapterDat.raw_content = j.at("content").get<std::vector<DocBlock>>();

    return chapterDat;
}
