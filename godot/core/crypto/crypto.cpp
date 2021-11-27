/*************************************************************************/
/*  crypto.cpp                                                           */
/*************************************************************************/
/*                       This file is part of:                           */
/*                           GODOT ENGINE                                */
/*                      https://godotengine.org                          */
/*************************************************************************/
/* Copyright (c) 2007-2021 Juan Linietsky, Ariel Manzur.                 */
/* Copyright (c) 2014-2021 Godot Engine contributors (cf. AUTHORS.md).   */
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

#include "crypto.h"

#include "core/engine.h"
#include "core/io/certs_compressed.gen.h"
#include "core/io/compression.h"

/// Resources

CryptoKey *(*CryptoKey::_create)() = nullptr;
CryptoKey *CryptoKey::create() {
	if (_create) {
		return _create();
	}
	return nullptr;
}

void CryptoKey::_bind_methods() {
	ClassDB::bind_method(D_METHOD("save", "path", "public_only"), &CryptoKey::save, DEFVAL(false));
	ClassDB::bind_method(D_METHOD("load", "path", "public_only"), &CryptoKey::load, DEFVAL(false));
	ClassDB::bind_method(D_METHOD("is_public_only"), &CryptoKey::is_public_only);
	ClassDB::bind_method(D_METHOD("save_to_string", "public_only"), &CryptoKey::save_to_string, DEFVAL(false));
	ClassDB::bind_method(D_METHOD("load_from_string", "string_key", "public_only"), &CryptoKey::load_from_string, DEFVAL(false));
}

X509Certificate *(*X509Certificate::_create)() = nullptr;
X509Certificate *X509Certificate::create() {
	if (_create) {
		return _create();
	}
	return nullptr;
}

void X509Certificate::_bind_methods() {
	ClassDB::bind_method(D_METHOD("save", "path"), &X509Certificate::save);
	ClassDB::bind_method(D_METHOD("load", "path"), &X509Certificate::load);
}

/// HMACContext

void HMACContext::_bind_methods() {
	ClassDB::bind_method(D_METHOD("start", "hash_type", "key"), &HMACContext::start);
	ClassDB::bind_method(D_METHOD("update", "data"), &HMACContext::update);
	ClassDB::bind_method(D_METHOD("finish"), &HMACContext::finish);
}

HMACContext *(*HMACContext::_create)() = nullptr;
HMACContext *HMACContext::create() {
	if (_create) {
		return _create();
	}
	ERR_FAIL_V_MSG(nullptr, "HMACContext is not available when the mbedtls module is disabled.");
}

/// Crypto

void (*Crypto::_load_default_certificates)(String p_path) = nullptr;
Crypto *(*Crypto::_create)() = nullptr;
Crypto *Crypto::create() {
	if (_create) {
		return _create();
	}
	ERR_FAIL_V_MSG(nullptr, "Crypto is not available when the mbedtls module is disabled.");
}

void Crypto::load_default_certificates(String p_path) {
	if (_load_default_certificates) {
		_load_default_certificates(p_path);
	}
}

PoolByteArray Crypto::hmac_digest(HashingContext::HashType p_hash_type, PoolByteArray p_key, PoolByteArray p_msg) {
	Ref<HMACContext> ctx = Ref<HMACContext>(HMACContext::create());
	ERR_FAIL_COND_V_MSG(ctx.is_null(), PoolByteArray(), "HMAC is not available witout mbedtls module.");
	Error err = ctx->start(p_hash_type, p_key);
	ERR_FAIL_COND_V(err != OK, PoolByteArray());
	err = ctx->update(p_msg);
	ERR_FAIL_COND_V(err != OK, PoolByteArray());
	return ctx->finish();
}

// Compares two HMACS for equality without leaking timing information in order to prevent timing attakcs.
// @see: https://paragonie.com/blog/2015/11/preventing-timing-attacks-on-string-comparison-with-double-hmac-strategy
bool Crypto::constant_time_compare(PoolByteArray p_trusted, PoolByteArray p_received) {
	const uint8_t *t = p_trusted.read().ptr();
	const uint8_t *r = p_received.read().ptr();
	int tlen = p_trusted.size();
	int rlen = p_received.size();
	// If the lengths are different then nothing else matters.
	if (tlen != rlen) {
		return false;
	}

	uint8_t v = 0;
	for (int i = 0; i < tlen; i++) {
		v |= t[i] ^ r[i];
	}
	return v == 0;
}

void Crypto::_bind_methods() {
	ClassDB::bind_method(D_METHOD("generate_random_bytes", "size"), &Crypto::generate_random_bytes);
	ClassDB::bind_method(D_METHOD("generate_rsa", "size"), &Crypto::generate_rsa);
	ClassDB::bind_method(D_METHOD("generate_self_signed_certificate", "key", "issuer_name", "not_before", "not_after"), &Crypto::generate_self_signed_certificate, DEFVAL("CN=myserver,O=myorganisation,C=IT"), DEFVAL("20140101000000"), DEFVAL("20340101000000"));
	ClassDB::bind_method(D_METHOD("sign", "hash_type", "hash", "key"), &Crypto::sign);
	ClassDB::bind_method(D_METHOD("verify", "hash_type", "hash", "signature", "key"), &Crypto::verify);
	ClassDB::bind_method(D_METHOD("encrypt", "key", "plaintext"), &Crypto::encrypt);
	ClassDB::bind_method(D_METHOD("decrypt", "key", "ciphertext"), &Crypto::decrypt);
	ClassDB::bind_method(D_METHOD("hmac_digest", "hash_type", "key", "msg"), &Crypto::hmac_digest);
	ClassDB::bind_method(D_METHOD("constant_time_compare", "trusted", "received"), &Crypto::constant_time_compare);
}

Crypto::Crypto() {
}

/// Resource loader/saver

RES ResourceFormatLoaderCrypto::load(const String &p_path, const String &p_original_path, Error *r_error) {
	String el = p_path.get_extension().to_lower();
	if (el == "crt") {
		X509Certificate *cert = X509Certificate::create();
		if (cert) {
			cert->load(p_path);
		}
		return cert;
	} else if (el == "key") {
		CryptoKey *key = CryptoKey::create();
		if (key) {
			key->load(p_path, false);
		}
		return key;
	} else if (el == "pub") {
		CryptoKey *key = CryptoKey::create();
		if (key) {
			key->load(p_path, true);
		}
		return key;
	}
	return nullptr;
}

void ResourceFormatLoaderCrypto::get_recognized_extensions(List<String> *p_extensions) const {
	p_extensions->push_back("crt");
	p_extensions->push_back("key");
	p_extensions->push_back("pub");
}

bool ResourceFormatLoaderCrypto::handles_type(const String &p_type) const {
	return p_type == "X509Certificate" || p_type == "CryptoKey";
}

String ResourceFormatLoaderCrypto::get_resource_type(const String &p_path) const {
	String el = p_path.get_extension().to_lower();
	if (el == "crt") {
		return "X509Certificate";
	} else if (el == "key" || el == "pub") {
		return "CryptoKey";
	}
	return "";
}

Error ResourceFormatSaverCrypto::save(const String &p_path, const RES &p_resource, uint32_t p_flags) {
	Error err;
	Ref<X509Certificate> cert = p_resource;
	Ref<CryptoKey> key = p_resource;
	if (cert.is_valid()) {
		err = cert->save(p_path);
	} else if (key.is_valid()) {
		String el = p_path.get_extension().to_lower();
		err = key->save(p_path, el == "pub");
	} else {
		ERR_FAIL_V(ERR_INVALID_PARAMETER);
	}
	ERR_FAIL_COND_V_MSG(err != OK, err, "Cannot save Crypto resource to file '" + p_path + "'.");
	return OK;
}

void ResourceFormatSaverCrypto::get_recognized_extensions(const RES &p_resource, List<String> *p_extensions) const {
	const X509Certificate *cert = Object::cast_to<X509Certificate>(*p_resource);
	const CryptoKey *key = Object::cast_to<CryptoKey>(*p_resource);
	if (cert) {
		p_extensions->push_back("crt");
	}
	if (key) {
		if (!key->is_public_only()) {
			p_extensions->push_back("key");
		}
		p_extensions->push_back("pub");
	}
}
bool ResourceFormatSaverCrypto::recognize(const RES &p_resource) const {
	return Object::cast_to<X509Certificate>(*p_resource) || Object::cast_to<CryptoKey>(*p_resource);
}
