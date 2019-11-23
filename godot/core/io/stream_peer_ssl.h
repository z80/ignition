/*************************************************************************/
/*  stream_peer_ssl.h                                                    */
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

#ifndef STREAM_PEER_SSL_H
#define STREAM_PEER_SSL_H

#include "core/io/stream_peer.h"

class StreamPeerSSL : public StreamPeer {
	GDCLASS(StreamPeerSSL, StreamPeer);

public:
	typedef void (*LoadCertsFromMemory)(const PoolByteArray &p_certs);

protected:
	static StreamPeerSSL *(*_create)();
	static void _bind_methods();

	static LoadCertsFromMemory load_certs_func;
	static bool available;

	bool blocking_handshake;

public:
	enum Status {
		STATUS_DISCONNECTED,
		STATUS_HANDSHAKING,
		STATUS_CONNECTED,
		STATUS_ERROR,
		STATUS_ERROR_HOSTNAME_MISMATCH
	};

	void set_blocking_handshake_enabled(bool p_enabled);
	bool is_blocking_handshake_enabled() const;

	virtual void poll() = 0;
	virtual Error accept_stream(Ref<StreamPeer> p_base) = 0;
	virtual Error connect_to_stream(Ref<StreamPeer> p_base, bool p_validate_certs = false, const String &p_for_hostname = String()) = 0;
	virtual Status get_status() const = 0;

	virtual void disconnect_from_stream() = 0;

	static StreamPeerSSL *create();

	static PoolByteArray get_cert_file_as_array(String p_path);
	static PoolByteArray get_project_cert_array();
	static void load_certs_from_file(String p_path);
	static void load_certs_from_memory(const PoolByteArray &p_memory);
	static bool is_available();

	StreamPeerSSL();
};

VARIANT_ENUM_CAST(StreamPeerSSL::Status);

#endif // STREAM_PEER_SSL_H
