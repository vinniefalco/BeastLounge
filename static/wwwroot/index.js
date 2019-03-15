function prepare_title_case(str) {
    return str.replace(/\w\S*/g, function(txt) {
        return txt.charAt(0).toUpperCase() + txt.substr(1).toLowerCase();
    })
}

document.addEventListener('DOMContentLoaded', function() {
    const JSON_AREA = document.getElementById("json_data")
    //Runs when page is loaded
    let API = window.location.protocol + "//" + window.location.host + "/api/http"

    function set_json_data(data) {
        const nav = JSON_AREA.children[0];
        const list  = JSON_AREA.children[1];

        for (let idx = 0; idx < data.length; idx++) {
            let new_link = document.createElement('a')
            new_link.text = prepare_title_case(data[idx].name)
            new_link.href = "#" + data[idx].name
            new_link.className = "nav-link json-data-nav-link"

            nav.appendChild(new_link)

            let new_list = document.createElement('div')
            new_list.id = data[idx].name
            new_list.className = "json-data-list target-sub-page"
            let new_list_text = document.createElement('textarea')

            new_list_text.value = JSON.stringify(data[idx], null, 4)
            new_list_text.rows = 7
            new_list_text.cols = 50
            new_list.appendChild(new_list_text)


            list.appendChild(new_list)
        }
    }

    fetch(API).then((response) => response.json())
              .then(set_json_data)
              .catch((error) => JSON_AREA.value = error)
})
