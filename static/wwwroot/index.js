document.addEventListener('DOMContentLoaded', function() {
    const JSON_AREA = document.getElementById("json_area")
    //Runs when page is loaded
    let API = window.location.protocol + "//" + window.location.host + "/api/http"

    fetch(API).then((response) => response.json())
              .then((response) => JSON_AREA.value = JSON.stringify(response, null, 4))
              .catch((error) => JSON_AREA.value = error)
})
