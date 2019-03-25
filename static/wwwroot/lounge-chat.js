"use strict"

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

    ws.addEventListener('open', (event) => {
        this.send_message("identify", { name: user_name } )
        this.send_message("join", { channel: 1 } )
        this.on_open()
    })

    ws.addEventListener('close', (event) => {
        this.on_close()
    })

    ws.addEventListener('error', (event) => {
        this.on_error(event)
    })

    ws.addEventListener('message', (event) => {
        try {
            this.on_message(JSON.parse(event.data))
        } catch (error) {
            this.on_error(error)
        }
    })
}
