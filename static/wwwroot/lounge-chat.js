//"use strict"

/**
 * Creates new chat by connecting via WS to uri
 *
 * @param uri Full URI to connect to
 * @param user_name Identity of user
 *
 * Callbacks:
 *
 * - on_open - Called when connectin is opened.
 * - on_close - Called when connection is closed.
 * - on_error - Called when error hapens. E.g. message is not valid JSON.
 * - on_message - Called when new message arrives.
 */
function LoungeChat(uri, user_name) {
    let ws = new WebSocket(uri)
    let id = 1

    this.send_message = function(method, params) {
        let json = {
            jsonrpc: "2.0",
            method: method,
            id: id,
            params: params,
        }

        ws.send(JSON.stringify(json))
        id++
    }

    this.disconnect = function() {
        ws.close()
    }

    this.on_open = function() {}
    this.on_close = function() {}
    this.on_error = function() {}
    this.on_message = function() {}

    ws.addEventListener('open', function(event) {
        this.send_message("identify", { cid: 1, name: user_name } )
        this.send_message("join", { cid: 2 } )
        this.on_open()
    }.bind(this))

    ws.addEventListener('close', function(event) {
        this.on_close()
    }.bind(this))

    ws.addEventListener('error', function(event) {
        this.on_error(event)
    }.bind(this))

    ws.addEventListener('message', function(event) {
        try {
            this.on_message(JSON.parse(event.data))
        } catch (error) {
            this.on_error(error)
        }
    }.bind(this))
}

//------------------------------------------------------------------------------

function CardText(card)
{
  var s = "AC2C3C4C5C6C7C8C9CTCJCQCKCAH2H3H4H5H6H7H8H9HTHJHQHKHAS2S3S4S5S6S7S8S9STSJSQSKSAD2D3D4D5D6D7D8D9DTDJDQDKD";
  return s.substring(2*(card-1), 2*card);
}

function UpdateSeat(jv, i, e) {
    var s;
    s = '[' + i + '] ';
         if(jv["state"] == "dealer")  s += "(Dealer)";
    else if(jv["state"] == "open")    s += "(Open)";
    else if(jv["state"] == "waiting") s += jv["user"] + " (Waiting)";
    else if(jv["state"] == "playing") s += jv["user"];
    else if(jv["state"] == "leaving") s += jv["user"] + " (Leaving)";
    e.textContent = s;
}

function UpdateTable(jv, e) {
    var seats = e.getElementsByClassName("seat");
    console.log(seats);
    for(var i = 0; i <= 5; i++)
        UpdateSeat(jv.seats[i], i, seats[i]);
}
