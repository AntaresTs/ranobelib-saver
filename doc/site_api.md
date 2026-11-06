

url: `https://ranobelib.me/ru/book/203736--seoul-object-story?section=chapters`

# GET `/api/manga/` - получение полной информации о тайтле

Пример с сайта
```html
https://api.imglib.info/api/manga/203736--seoul-object-story?fields[0]=background&fields[1]=eng_name&fields[2]=otherNames&fields[3]=summary&fields[4]=releaseDate&fields[5]=country&fields[6]=type_id&fields[7]=caution&fields[8]=views&fields[9]=close_view&fields[10]=rate_avg&fields[11]=rate&fields[12]=genres&fields[13]=tags&fields[14]=teams&fields[15]=user&fields[16]=franchise&fields[17]=authors&fields[18]=publisher&fields[19]=userRating&fields[20]=moderated&fields[21]=metadata&fields[22]=metadata.count&fields[23]=metadata.close_comments&fields[24]=translation_quality_rating&fields[25]=manga_status_id&fields[26]=chap_count&fields[27]=status_id&fields[28]=extra_release_status&fields[29]=artists&fields[30]=format
```

Развернутый
```
scheme:	https
host: api.imglib.info
filename: /api/manga/203736--seoul-object-story

fields[0]: background
fields[1]: eng_name
fields[2]: otherNames
fields[3]: summary
fields[4]: releaseDate
fields[5]: country
fields[6]: type_id
fields[7]: caution
fields[8]: views
fields[9]: close_view
fields[10]: rate_avg
fields[11]: rate
fields[12]: genres
fields[13]:	tags
fields[14]:	teams
fields[15]:	user
fields[16]:	franchise
fields[17]:	authors
fields[18]:	publisher
fields[19]:	userRating
fields[20]:	moderated
fields[21]:	metadata
fields[22]:	metadata.count
fields[23]:	metadata.close_comments
fields[24]:	translation_quality_rating
fields[25]:	manga_status_id
fields[26]:	chap_count
fields[27]:	status_id
fields[28]:	extra_release_status
fields[29]:	artists
fields[30]:	format
```

## JSON
Интересные поля

- data
    - ["data"]["id"]
    - ["data"]["name"] - Название 
    - ["data"]["rus_name"] - Название на русском
    - ["data"]["otherNames"] - массив с альтернативными названиями
    - ["data"]["slug_url"] - текстовый id произведения, тот что в url
    - ["data"]["cover"] - обложки
        - ["thumbnail"]
        - ["default"]
        - ["md"]
    - `ageRestriction` - возрастной рейтинг
        - `id` - цифра
        - `label` - буквенный (16+, 18+)
    - `type` - тип (вероятно откуда, пример: type.label = "Корея")
        - `id`
        - `label`
    - `summary` - описание
        - `type` - тип контента, например "doc"
        - `content` - сам контент, с разделением по параграфрам
            - `0`, `1`, ... - номер параграфа
                - `type` - тип контента (например "text")
                - `text` - параграф
    - releaseDate - дата публиуации
    - teams - команды участвующие в переводе
        - 0, 1, ...
            - name - ник автора
    - genres
        - 0, ... - номера
            - name - название жанра
    - tags - теги
        - 0, ... - номера
            - name - название тегов
    - publisher - Издатель
        - name - ник издателя
    - franchise - Франшиза (фендом)
        - name - Название
    - authors - Авторы 
        - 0, ...
            - name - имя автора
    - artists - Художники
        - 0, ...
            - name - названия



# GET - Получение списка глав и branch_id
https://api.imglib.info/api/manga/{Title_text_id}/chapters

## JSON

- data
    - 0, ....
        - id
        - index
        - item_number
        - volume
        - number
        - number_secondary
        - name
        - branches_count
        - branches
            - 0, ...
                - id
                - branch_id
                - created_at
                - teams
                    - 0, ....
                        - name
                - expired_type
