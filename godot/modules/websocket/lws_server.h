/*************************************************************************/
/*  lws_server.h                                                         */
/*************************************************************************/
/*                       This file is part of:                           */
/*                           GODOT ENGINE                                */
/*                      https://godotengine.org                          */
/*************************************************************************/
/* Copyright (c) 2007-2019 Juan Linietsky, Ariel Manzur.                 */
/* Copyright (c) 2014-2019 Godot Engine contributors (cf. AUTHORS.md)    */
/*                                                                       */
/* Permission is hereby granted, free of charge, to any person obtaining */
/* a copy of this software and associated documentation files (the       */
/* "Software"), to deal in the Software without restriction, including   */
/* without limitation the rights to use, copy, modify, merge, publish,   */
/* distribute, sublicense, and/or sell copies of the Software, and to    */
/* permit persons to whom the Software is furnished to do so, subject to */
/* the following conditions:                                             */
/*                                                                       */
/* The above copyright notice and this permission notice shall be        */
/* included in all copies or substantial portions of the Software.       */
/*                                                                       */
/* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,       */
/* EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF    */
/* MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.*/
/* IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY  */
/* CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,  */
/* TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE     */
/* SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.                */
/*************************************************************************/

#ifndef LWSSERVER_H
#define LWSSERVER_H

#ifndef JAVASCRIPT_ENABLED

#include "core/reference.h"
#include "lws_helper.h"
#include "lws_peer.h"
#include "websocket_server.h"

class LWSServer : public WebSocketServer {

	GDCIIMPL(LWSServer, WebSocketServer);

	LWS_HELPER(LWSServer);

private:
	Map<int, Ref<LWSPeer> > peer_map;
	int _in_buf_size;
	int _in_pkt_size;
	int _out_buf_size;
	int _out_pkt_size;

public:
	Error listen(int p_port, PoolVector<String> p_protocols = PoolVector<String>(), bool gd_mp_api = false);
	void stop();
	bool is_listening() const;
	int get_max_packet_size() const;
	bool has_peer(int p_id) const;
	Ref<WebSocketPeer> get_peer(int p_id) const;
	IP_Address get_peer_address(int p_peer_id) const;
	int get_peer_port(int p_peer_id) const;
	void disconnect_peer(int p_peer_id, int p_code = 1000, String p_reason = "");
	virtual void poll() { _lws_poll(); }

	LWSServer();
	~LWSServer();
};

#endif // JAVASCRIPT_ENABLED

#endif // LWSSERVER_H
