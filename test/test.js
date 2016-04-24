var assert = require("chai").assert;
var Auth = require("../ctap-auth.js");

describe("Basic tests", function() {
	it("does init", function() {
		// this.timeout(5000);
		var a = new Auth("./lib/ctap-auth.dylib");
		assert.isFunction (a.init);
		assert.isFunction (a.shutdown);
		assert.isFunction (a.sendCborMessage);
		assert.isFunction (a.receiveCborMessage);
		assert.isFunction (a.lib.init);
		assert.isFunction (a.lib.shutdown);
		assert.isFunction (a.lib.sendCborMessage);
		assert.isFunction (a.lib.receiveCborMessage);
		assert.isString (a.libName);
		assert(a.initDone);
	});

	it("does shutdown", function() {
		var a = new Auth("./lib/ctap-auth.dylib");
		assert.equal (a.shutdown(), 0);
		assert.isFalse(a.initDone);
	});

	it("send message", function(done) {
		var a = new Auth("./lib/ctap-auth.dylib");
		a.sendCborMessage (new Buffer("testing 1 2 3\0"), done);
	});

	it("get message", function(done) {
		this.timeout(15000);
		var a = new Auth("./lib/ctap-auth.dylib");
		a.receiveCborMessage (function (err, res) {
			console.log ("a.receiveCborMessage done in Mocha:", res);
			done(err, res);
		});
		a.sendCborMessage (new Buffer("beer please\0"), function (err, res) {
			console.log ("Done sending message");
		});
	});
});