var ffi = require("ffi");
var ref = require("ref");
var async = require("async");

module.exports = CtapAuth;

// TODO: might want to convert this to an IIFE
function CtapAuth(library, params) {
	if (typeof library !== "string") {
		return "typeError: expected 'library' argument to be string path to dynamic library";
	}
	this.libName = library;

	var intPtr = ref.refType('int');

	this.lib = ffi.Library(library, {
		'init': ["int", ["pointer"]],
		'sendCborMessage': ["int", ["pointer", "int"]],
		"receiveCborMessage": ["pointer", [intPtr]],
		"shutdown": ["int", ["pointer"]]
	});

	var ret = this.init(params);
	if (ret !== 0) {
		throw (Error (this.libName + ": init failed with error: " + ret));
	}
}

CtapAuth.prototype.init = function(params) {
	// TODO: pass params as ref-struct
	if (params === undefined) params = null;

	var ret = this.lib.init(params);
	if (ret !== 0) {
		console.log(this.libName + ": init failed with value: " + ret);
	}
	this.initDone = true;

	return ret;
};

CtapAuth.prototype.shutdown = function(params) {
	// TODO: pass params as ref-struct
	if (params === undefined) params = null;

	var ret = this.lib.shutdown(params);
	if (ret !== 0) {
		console.log(this.libName + ": shutdown failed with value: " + res);
	}
	this.initDone = false;

	return ret;
};

CtapAuth.prototype.sendCborMessage = function(msg, cb) {
	// TODO: msg may be one of: Array, TypedArray or Buffer
	this.lib.sendCborMessage.async (msg, msg.length, function (err, res) {
		if (err) return cb (err);

		cb (err, res);
	});
};

CtapAuth.prototype.receiveCborMessage = function(cb) {
	// TODO: msg may be one of: Array, TypedArray or Buffer
	// var buf = new Buffer(1024);
	var retLen = ref.alloc('int', 7);

	this.lib.receiveCborMessage.async (retLen, function (err, res) {
		if (err) return cb (err);
		var len = retLen.deref();

		if (res.isNull()) {
			console.log ("receiveCborMessage: received NULL message");
			cb (Error ("receiveCborMessage: received NULL message"));
		}
		res = ref.reinterpret (res, len, 0);
		console.log ("receiveCborMessage:", res.toString());
		cb (err, res);
	});
};