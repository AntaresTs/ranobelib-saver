#pragma once

#include <string>
#include <vector>
#include <cstdint>

// ========================| descroptor |============================
    // ======== | application/other | ========= 
struct AppOther {
    std::string title;
    std::string homepage;
    std::vector<std::string> tags;
    std::string section;
    std::string description;
    std::string category = "application/other";
};


// =============================
// | literature/book |
// =============================
struct LibBook {
    // ==================== ИДЕНТИФИКАЦИЯ ====================
    std::string title;                        // Основное название произведения (без указания тома/издания)
    std::string original_title;               // Оригинальное название (если книга переводная)
    std::vector<std::string> alternative_titles; // Альтернативные названия, переводы названия, сокращения
    std::vector<std::string> authors;         // Авторы произведения (ФИО в едином формате)
    std::vector<std::string> co_authors;      // Соавторы, иллюстраторы, составители (если нужно отделить от основных авторов)
    std::string category = "literature/book"; // Тип литературы: literature/book, literature/article, literature/journal, literature/magazine, literature/novel, literature/poem, literature/essay и т.д. (можно добавлять подтипы через "/")

    // ==================== КЛАССИФИКАЦИЯ ====================
    std::vector<std::string> tags;            // Теги — свободные метки для поиска и группировки
    std::vector<std::string> genres;          // Жанры
    std::vector<std::string> themes;          // Основные темы произведения: любовь, война, взросление и т.п.
    std::vector<std::string> keywords;        // Ключевые слова для расширенного поиска

    // ==================== ОПИСАНИЕ ====================
    std::string description;                  // Краткое описание/аннотация произведения
    std::string summary;                      // Расширенное содержание (спойлерное или полное)
    std::vector<std::string> quotes;          // Известные цитаты из произведения
    std::string dedication;                   // Посвящение (если есть)

    // ==================== ИЗДАНИЕ И ПУБЛИКАЦИЯ ====================
    std::string year_of_publication;          // Год первой публикации произведения (не конкретного издания)
    std::string original_language;            // Язык оригинала
    std::vector<std::string> languages;       // Языки, на которые переведено произведение
    std::vector<std::string> isbn;            // ISBN всех хранимых изданий
    std::string series;                       // Название серии/цикла, к которому относится произведение
    int total_volume = 0;                     // Общее количество томов/частей в произведении (0 — если не применимо)
    std::string publisher_info;               // Общая информация об издателях (без привязки к конкретному файлу)

    // ==================== СВЯЗИ И ИСТОЧНИКИ ====================
    std::string source_link;                  // Ссылка на первоисточник/официальную страницу произведения
    std::vector<std::string> related_works;   // Связанные произведения (сиквелы, приквелы, спин-оффы)
    std::vector<std::string> adaptations;     // Экранизации, аудиокниги, игры по мотивам
    std::vector<std::string> references;      // Ссылки на внешние базы (Goodreads, Fantlab, Wikipedia и т.п.)

    // ==================== ПРАВОВОЙ СТАТУС ====================
    std::string copyright_status = "unknown";             // Статус авторских прав: public_domain, copyrighted, unknown
    std::string license = "unknown";                      // Лицензия распространения (если применимо)

    // ==================== ПРОЧЕЕ ====================
    std::vector<std::string> awards;          // Награды и премии, полученные произведением
    std::string notes;                        // Любые дополнительные заметки
    std::vector<std::string> external_ids;    // Внешние идентификаторы (OpenLibrary ID, Wikidata ID и т.п.)
}


/// ========================
///        Literature Conix
/// ========================

struct LibComix {
    // ==================== ИДЕНТИФИКАЦИЯ ====================
    std::string title;                            // str — Основное название произведения (без указания выпуска/тома/издания)
    std::string original_title;                   // str — Оригинальное название (если комикс переводной)
    std::vector<std::string> alternative_titles;  // list[str] — Альтернативные названия, переводы названия, сокращения
    std::vector<std::string> authors;             // list[str] — Авторы произведения: сценаристы, писатели (ФИО/псевдонимы)
    std::vector<std::string> co_authors;          // list[str] — Соавторы: художники, райтеры, колористы, леттереры, переводчики
    std::string category = "literature/comix";    // str — Тип: literature/comix, literature/comix/manga, literature/comix/manhwa,
                                                  //   literature/comix/manhua, literature/comix/webtoon, literature/comix/graphic-novel
                                                  //   Подтипы добавляются через "/"

    // ==================== КЛАССИФИКАЦИЯ ====================
    std::vector<std::string> tags;                // list[str] — Теги (ОБЯЗАТЕЛЬНОЕ ПОЛЕ) — свободные метки для поиска и группировки
    std::vector<std::string> genres;              // list[str] — Жанры: action, romance, fantasy, horror, slice-of-life и т.п.
    std::vector<std::string> themes;              // list[str] — Основные темы: дружба, взросление, война, школа и т.п.
    std::vector<std::string> keywords;            // list[str] — Ключевые слова для расширенного поиска
    std::vector<std::string> demographics;        // list[str] — Демографическая целевая группа оригинала: shounen, shoujo, seinen,
                                                  //   josei, kodomomuke, kids, teen, adult (для манги/манхвы/маньхуа)

    // ==================== ОПИСАНИЕ ====================
    std::string description;                      // str — Краткое описание/аннотация произведения
    std::string summary;                          // str — Расширенное содержание (спойлерное или полное)
    std::vector<std::string> quotes;              // list[str] — Известные цитаты/реплики из произведения
    std::string dedication;                       // str — Посвящение (если есть)

    // ==================== ИЗДАНИЕ И ПУБЛИКАЦИЯ ====================
    std::string year_of_publication;              // str — Год первой публикации произведения (не конкретного издания)
    std::string original_language;                // str — Язык оригинала (ja, ko, zh, en и т.п.)
    std::vector<std::string> languages;           // list[str] — Языки, на которые переведено произведение
    std::vector<std::string> isbn;                // list[str] — ISBN всех хранимых изданий
    std::string series;                           // str — Название серии/цикла/франшизы
    int total_volume = 0;                         // int — Общее количество томов/частей произведения (0 — если не применимо)
    int total_chapters = 0;                       // int — Общее количество глав/выпусков (0 — если неизвестно/не применимо)
    std::string status;                           // str — Статус произведения: ongoing, completed, hiatus, cancelled, unknown
    std::string publisher_info;                   // str — Общая информация об издателях (без привязки к конкретному файлу)

    // ==================== СВЯЗИ И ИСТОЧНИКИ ====================
    std::string source_link;                      // str — Ссылка на первоисточник/официальную страницу произведения
    std::vector<std::string> references;          // list[str] — Ссылки на внешние базы (MangaDex, MyAnimeList, AniList, Comic Vine,
                                                  //   Goodreads, Wikipedia, Wikidata и т.п.)
    std::vector<std::string> external_ids;        // list[str] — Внешние идентификаторы (MAL ID, AniList ID, MangaDex ID, Wikidata ID)

    // ==================== ПРАВОВОЙ СТАТУС ====================
    std::string copyright_status = "unknown";     // str — Статус авторских прав: public_domain, copyrighted, unknown
    std::string license = "unknown";              // str — Лицензия распространения (если применимо)
};

