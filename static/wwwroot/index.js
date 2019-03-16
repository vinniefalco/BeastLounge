function prepare_title_case(str) {
    return str.replace(/\w\S*/g, function(txt) {
        return txt.charAt(0).toUpperCase() + txt.substr(1).toLowerCase();
    })
}

function create_table_cell(text) {
    let cell = document.createElement('td')
    cell.className = "c-table__cell"
    cell.innerText = text
    return cell
}

document.addEventListener('DOMContentLoaded', function() {
    //Runs when page is loaded
    const JSON_TABLE = document.getElementById("json_table")
    let API = window.location.protocol + "//" + window.location.host + "/api/http"

    function set_json_data(data) {
        for (let idx = 0; idx < data.length; idx++) {
            let row = document.createElement('tr')
            row.className = "c-table__row"

            row.appendChild(create_table_cell(prepare_title_case(data[idx].name)))
            row.appendChild(create_table_cell(prepare_title_case(data[idx].type)))
            row.appendChild(create_table_cell(data[idx].address))
            row.appendChild(create_table_cell(data[idx].port_num))
            row.appendChild(create_table_cell(data[idx].sessions))

            JSON_TABLE.appendChild(row)
        }
    }

    fetch(API).then((response) => response.json())
              .then(set_json_data)
              .catch((error) => alert(error))
})
