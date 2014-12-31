//###############################################################
//# project: librootcoin										#
//# author:  bitsta												#
//# date:    26.11.2014											#
//# spec:    Type-definition for librootcoin. 					#
//# version: 0.1 - THIS IS EXPERIMENTAL SOFTWARE! 				#
//###############################################################

#include "librootcoin.h"

#include <string>
#include <stdexcept>

#include <jsonrpccpp/client.h>
#include <jsonrpccpp/client/connectors/httpclient.h>

using jsonrpc::Client;
using jsonrpc::JSONRPC_CLIENT_V1;

using jsonrpc::HttpClient;
using jsonrpc::JsonRpcException;

using Json::Value;
using Json::ValueIterator;

using std::map;
using std::string;
using std::vector;


libRootcoin::libRootcoin(){
	conn = {};
}

libRootcoin::libRootcoin(const string& user, const string& password, const string& host, int port){
	conn.user = user;
	conn.password = password;
	conn.host = host;
	conn.port = port;
	conn.url = "http://" + conn.user + ":" + conn.password + "@" + conn.host + ":" + NumberToString(conn.port);
}

bool libRootcoin::IsInit(){
	return !(conn.user.empty() || conn.password.empty() || conn.host.empty() || conn.port == 0);
}

string libRootcoin::NumberToString (int number){
	std::ostringstream ss;
	ss << number;
	return ss.str();
}

int libRootcoin::StringToNumber (const string &text){
	std::istringstream ss(text);
	int result;
	return ss >> result ? result : 0;
}

Value libRootcoin::sendcommand(const string& command, const Value& params){    
    HttpClient client(conn.url);
    client.SetTimeout(15000);

    Client c(client, JSONRPC_CLIENT_V1);
    Value result;

    try{
		result = c.CallMethod(command, params);
	}
	catch (JsonRpcException& e){
		RootcoinException err(e.GetCode(), e.GetMessage());
		throw err;
	}

	return result;
}


/* === General functions === */
getinfo_t libRootcoin::getinfo() {
	string command = "getinfo";
	Value params, result;
	getinfo_t ret;
	result = sendcommand(command, params);

	ret.version = result["version"].asInt();
	ret.protocolversion = result["protocolversion"].asInt();
	ret.walletversion = result["walletversion"].asInt();
	ret.balance = result["balance"].asDouble();
	ret.blocks = result["blocks"].asInt();
	ret.timeoffset = result["timeoffset"].asInt();
	ret.connections = result["connections"].asInt();
	ret.proxy = result["proxy"].asString();
	ret.difficulty = result["difficulty"].asDouble();
	ret.testnet = result["testnet"].asBool();
	ret.keypoololdest = result["keypoololdest"].asInt();
	ret.keypoolsize = result["keypoolsize"].asInt();
	ret.paytxfee = result["paytxfee"].asDouble();
	ret.unlocked_until = result["unlocked_until"].asInt();
	ret.errors = result["errors"].asString();

	return ret;
}

void libRootcoin::stop() {
	string command = "stop";
	Value params;
	sendcommand(command, params);
}

/* === Node functions === */
void libRootcoin::addnode(const string& node, const string& comm) {

	if (!(comm == "add" || comm == "remove" || comm == "onetry")) {
		throw std::runtime_error("Incorrect addnode parameter: " + comm);
	}

	string command = "addnode";
	Value params;
	params.append(node);
	params.append(comm);
	sendcommand(command, params);
}

vector<nodeinfo_t> libRootcoin::getaddednodeinfo(bool dns) {
	string command = "getaddednodeinfo";
	Value params, result;
	vector<nodeinfo_t> ret;

	params.append(dns);
	result = sendcommand(command, params);

	for (ValueIterator it1 = result.begin(); it1 != result.end(); it1++) {
		Value val1 = (*it1);
		nodeinfo_t node;

		node.addednode = val1["addednode"].asString();

		if (dns) {
			node.connected = val1["connected"].asBool();

			for (ValueIterator it2 = val1["addresses"].begin(); it2 != val1["addresses"].end(); it2++) {
				Value val2 = (*it2);
				netaddress_t net;

				net.address = val2["address"].asString();

				//TODO: Bug in here. Always shows true instead of false.
				net.connected = val2["connected"].asBool();

				node.addresses.push_back(net);
			}
		}
		ret.push_back(node);
	}

	return ret;
}

vector<nodeinfo_t> libRootcoin::getaddednodeinfo(bool dns, const std::string& node) {
	string command = "getaddednodeinfo";
	Value params, result;
	vector<nodeinfo_t> ret;

	params.append(dns);
	params.append(node);
	result = sendcommand(command, params);

	for (ValueIterator it1 = result.begin(); it1 != result.end(); it1++) {
		Value val1 = (*it1);
		nodeinfo_t node;

		node.addednode = val1["addednode"].asString();

		if (dns) {
			node.connected = val1["connected"].asBool();

			for (ValueIterator it2 = val1["addresses"].begin(); it2 != val1["addresses"].end(); it2++) {
				Value val2 = (*it2);
				netaddress_t net;

				net.address = val2["address"].asString();
				net.connected = val2["connected"].asBool();

				node.addresses.push_back(net);
			}
		}

		ret.push_back(node);
	}

	return ret;
}

int libRootcoin::getconnectioncount() {
	string command = "getconnectioncount";
	Value params, result;
	result = sendcommand(command, params);

	return result.asInt();
}

vector<peerinfo_t> libRootcoin::getpeerinfo() {
	string command = "getpeerinfo";
	Value params, result;
	vector<peerinfo_t> ret;
 	result = sendcommand(command, params);

	for(ValueIterator it = result.begin(); it != result.end(); it++){
		Value val = (*it);
		peerinfo_t peer;

		peer.addr = val["addr"].asString();
		peer.services = val["services"].asString();
		peer.lastsend = val["lastsend"].asInt();
		peer.lastrecv = val["lastrecv"].asInt();
		peer.bytessent = val["bytessent"].asInt();
		peer.bytesrecv = val["bytesrecv"].asInt();
		peer.conntime = val["conntime"].asInt();
		peer.pingtime = val["pingtime"].asDouble();
		peer.version = val["version"].asInt();
		peer.subver = val["subver"].asString();
		peer.inbound = val["inbound"].asBool();
		peer.startingheight = val["startingheight"].asInt();
		peer.banscore = val["banscore"].asInt();

		ret.push_back(peer);
	}

	return ret;
}

/* === Wallet functions === */
void libRootcoin::backupwallet(const string& destination) {
	string command = "backupwallet";
	Value params;
	params.append(destination);
	sendcommand(command, params);
}

void libRootcoin::encryptwallet(const string& passphrase) {
	string command = "encryptwallet";
	Value params;
	params.append(passphrase);
	sendcommand(command, params);
}

void libRootcoin::walletlock() {
	string command = "walletlock";
	Value params;
	sendcommand(command, params);
}

void libRootcoin::walletpassphrase(const string& passphrase, int timeout) {
	string command = "walletpassphrase";
	Value params;
	params.append(passphrase);
	params.append(timeout);
	sendcommand(command, params);
}

void libRootcoin::walletpassphrasechange(const string& oldpassphrase, const string& newpassphrase) {
	string command = "walletpassphrasechange";
	Value params;
	params.append(oldpassphrase);
	params.append(newpassphrase);
	sendcommand(command, params);
}

string libRootcoin::getkey(const string& rootcoinaddress) {
	string command = "dumpprivkey";
	Value params, result;
	params.append(rootcoinaddress);
	result = sendcommand(command, params);
	return result.asString();
}

void libRootcoin::importprivkey(const string& rootcoinprivkey) {
	string command = "importprivkey";
	Value params;
	params.append(rootcoinprivkey);
	sendcommand(command, params);
}

void libRootcoin::importprivkey(const string& rootcoinprivkey, const string& label, bool rescan) {
	string command = "importprivkey";
	Value params;
	params.append(rootcoinprivkey);
	params.append(label);
	params.append(rescan);
	sendcommand(command, params);
}

string libRootcoin::addmultisigaddress(int nrequired, const vector<string>& keys) {
	string command = "addmultisigaddress";
	Value params, result;

	Value arrParam(Json::arrayValue);
	for(vector<string>::const_iterator it = keys.begin(); it != keys.end(); it++){
		arrParam.append(*it);
	}

	params.append(nrequired);
	params.append(arrParam);
	result = sendcommand(command, params);
	return result.asString();
}

string libRootcoin::addmultisigaddress(int nrequired, const vector<string>& keys, const string& account) {
	string command = "addmultisigaddress";
	Value params, result;
	params.append(nrequired);

	Value arrParam(Json::arrayValue);
	for(vector<string>::const_iterator it = keys.begin(); it != keys.end(); it++){
		arrParam.append(*it);
	}

	params.append(arrParam);
	params.append(account);
	result = sendcommand(command, params);
	return result.asString();
}

multisig_t libRootcoin::createmultisig(int nrequired, const vector<string>& keys) {
	string command = "createmultisig";
	Value params, result;
	multisig_t ret;
	params.append(nrequired);

	Value arrParam(Json::arrayValue);
	for(vector<string>::const_iterator it = keys.begin(); it != keys.end(); it++){
		arrParam.append(*it);
	}

	params.append(arrParam);
	result = sendcommand(command, params);

	ret.address = result["address"].asString();
	ret.redeemScript = result["redeemScript"].asString();

	return ret;
}

string libRootcoin::getnewaddress(const string& account) {
	string command = "getnewaddress";
	Value params, result;
	params.append(account);
	result = sendcommand(command, params);
	return result.asString();
}

validateaddress_t libRootcoin::validateaddress(const string& rootcoinaddress) {
	string command = "validateaddress";
	Value params, result;
	validateaddress_t ret;

	params.append(rootcoinaddress);
	result = sendcommand(command, params);

	ret.isvalid = result["isvalid"].asBool();
	ret.address = result["address"].asString();
	ret.ismine = result["ismine"].asBool();
	ret.isscript = result["isscript"].asBool();
	ret.pubkey = result["pubkey"].asString();
	ret.iscompressed = result["iscompressed"].asBool();

	return ret;
}

void libRootcoin::keypoolrefill() {
	string command = "keypoolrefill";
	Value params;
	sendcommand(command, params);
}

bool libRootcoin::settxfee(double amount) {
	string command = "settxfee";
	Value params, result;
	params.append(amount);
	result = sendcommand(command, params);
	return result.asBool();
}

string libRootcoin::signmessage(const std::string& rootcoinaddress, const std::string& message) {
	string command = "signmessage";
	Value params, result;
	params.append(rootcoinaddress);
	params.append(message);
	result = sendcommand(command, params);
	return result.asString();
}

bool libRootcoin::verifymessage(const std::string& rootcoinaddress, const std::string& signature, const std::string& message) {
	string command = "verifymessage";
	Value params, result;
	params.append(rootcoinaddress);
	params.append(signature);
	params.append(message);
	result = sendcommand(command, params);
	return result.asBool();
}

/* === Accounting === */
double libRootcoin::getbalance() {
	string command = "getbalance";
	Value params, result;
	result = sendcommand(command, params);

	return result.asDouble();
}

double libRootcoin::getbalance(const string& account, int minconf) {
	string command = "getbalance";
	Value params, result;
	params.append(account);
	params.append(minconf);
	result = sendcommand(command, params);

	return result.asDouble();
}

double libRootcoin::getunconfirmedbalance() {
	string command = "getunconfirmedbalance";
	Value params, result;
	result = sendcommand(command, params);

	return result.asDouble();
}

double libRootcoin::getreceivedbyaccount(const string& account, int minconf) {
	string command = "getreceivedbyaccount";
	Value params, result;
	params.append(account);
	params.append(minconf);
	result = sendcommand(command, params);

	return result.asDouble();
}

double libRootcoin::getreceivedbyaddress(const string& rootcoinaddress, int minconf) {
	string command = "getreceivedbyaddress";
	Value params, result;
	params.append(rootcoinaddress);
	params.append(minconf);
	result = sendcommand(command, params);

	return result.asDouble();
}

vector<accountinfo_t> libRootcoin::listreceivedbyaccount(int minconf, bool includeempty) {
	string command = "listreceivedbyaccount";
	Value params, result;
	vector<accountinfo_t> ret;

	params.append(minconf);
	params.append(includeempty);
	result = sendcommand(command, params);

	for (ValueIterator it = result.begin(); it != result.end(); it++) {
		Value val = (*it);
		accountinfo_t acct;
		acct.account = val["account"].asString();
		acct.amount = val["amount"].asDouble();
		acct.confirmations = val["confirmations"].asInt();

		ret.push_back(acct);
	}

	return ret;
}

vector<addressinfo_t> libRootcoin::listreceivedbyaddress(int minconf, bool includeempty) {
	string command = "listreceivedbyaddress";
	Value params, result;
	vector<addressinfo_t> ret;

	params.append(minconf);
	params.append(includeempty);
	result = sendcommand(command, params);

	for (ValueIterator it1 = result.begin(); it1 != result.end(); it1++) {
		Value val = (*it1);
		addressinfo_t addr;
		addr.address = val["address"].asString();
		addr.account = val["account"].asString();
		addr.amount = val["amount"].asDouble();
		addr.confirmations = val["confirmations"].asInt();

		for (ValueIterator it2 = val["txids"].begin(); it2 != val["txids"].end(); it2++) {
			addr.txids.push_back((*it2).asString());
		}

		ret.push_back(addr);
	}

	return ret;
}

gettransaction_t libRootcoin::gettransaction(const string& tx) {
	string command = "gettransaction";
	Value params, result;
	gettransaction_t ret;
	params.append(tx);
	result = sendcommand(command, params);

	ret.amount = result["amount"].asDouble();
	ret.confirmations = result["confirmations"].asInt();
	ret.blockhash = result["blockhash"].asString();
	ret.blockindex = result["blockindex"].asInt();
	ret.blocktime = result["blocktime"].asInt();
	ret.txid = result["txid"].asString();

	for (ValueIterator it = result["walletconflicts"].begin();
			it != result["walletconflicts"].end(); it++) {
		ret.walletconflicts.push_back((*it).asString());
	}

	ret.time = result["time"].asInt();
	ret.timereceived = result["timereceived"].asInt();

	for (ValueIterator it = result["details"].begin();
			it != result["details"].end(); it++) {
		Value val = (*it);
		transactiondetails_t tmp;
		tmp.account = val["account"].asString();
		tmp.address = val["address"].asString();
		tmp.category = val["category"].asString();
		tmp.amount = val["amount"].asDouble();

		ret.details.push_back(tmp);
	}

	ret.hex = result["hex"].asString();

	return ret;
}

vector<transactioninfo_t> libRootcoin::listtransactions() {
	string command = "listtransactions";
	Value params, result;
	vector<transactioninfo_t> ret;

	result = sendcommand(command, params);

	for (ValueIterator it = result.begin(); it != result.end(); it++) {
		Value val = (*it);
		transactioninfo_t tmp;

		tmp.account = val["account"].asString();
		tmp.address = val["address"].asString();
		tmp.category = val["category"].asString();
		tmp.amount = val["amount"].asDouble();
		tmp.confirmations = val["confirmations"].asInt();
		tmp.blockhash = val["blockhash"].asString();
		tmp.blockindex = val["blockindex"].asInt();
		tmp.blocktime = val["blocktime"].asInt();
		tmp.txid = val["txid"].asString();

		for (ValueIterator it2 = val["walletconflicts"].begin();
				it2 != val["walletconflicts"].end(); it2++) {
			tmp.walletconflicts.push_back((*it2).asString());
		}

		tmp.time = val["time"].asInt();
		tmp.timereceived = val["timereceived"].asInt();

		ret.push_back(tmp);
	}

	return ret;
}

vector<transactioninfo_t> libRootcoin::listtransactions(const string& account, int count, int from) {
	string command = "listtransactions";
	Value params, result;
	vector<transactioninfo_t> ret;

	params.append(account);
	params.append(count);
	params.append(from);
	result = sendcommand(command, params);

	for (ValueIterator it = result.begin(); it != result.end(); it++) {
		Value val = (*it);
		transactioninfo_t tmp;

		tmp.account = val["account"].asString();
		tmp.address = val["address"].asString();
		tmp.category = val["category"].asString();
		tmp.amount = val["amount"].asDouble();
		tmp.confirmations = val["confirmations"].asInt();
		tmp.blockhash = val["blockhash"].asString();
		tmp.blockindex = val["blockindex"].asInt();
		tmp.blocktime = val["blocktime"].asInt();
		tmp.txid = val["txid"].asString();

		for (ValueIterator it2 = val["walletconflicts"].begin();
				it2 != val["walletconflicts"].end(); it2++) {
			tmp.walletconflicts.push_back((*it2).asString());
		}

		tmp.time = val["time"].asInt();
		tmp.timereceived = val["timereceived"].asInt();

		ret.push_back(tmp);
	}

	return ret;
}

string libRootcoin::getaccount(const string& rootcoinaddress) {
	string command = "getaccount";
	Value params, result;
	params.append(rootcoinaddress);
	result = sendcommand(command, params);
	return result.asString();
}

string libRootcoin::getaccountaddress(const string& account) {
	string command = "getaccountaddress";
	Value params, result;
	params.append(account);
	result = sendcommand(command, params);
	return result.asString();
}


vector<std::string> libRootcoin::getaddressesbyaccount(const string& account) {
	string command = "getaddressesbyaccount";
	Value params, result;
	vector<string> ret;

	params.append(account);
	result = sendcommand(command, params);

	for(ValueIterator it = result.begin(); it != result.end(); it++){
		ret.push_back((*it).asString());
	}

	return ret;
}

map<string, double> libRootcoin::listaccounts(int minconf) {
	string command = "listaccounts";
	Value params, result;
	Value account, amount;
	map<string, double> ret;

	params.append(minconf);
	result = sendcommand(command, params);

	for(ValueIterator it = result.begin(); it != result.end(); it++){
		Value val = (*it);
		std::pair<string, double> tmp;

		tmp.first = it.key().asString();
		tmp.second = result[tmp.first].asDouble();
		ret.insert(tmp);
	}

	return ret;
}

vector< vector<addressgrouping_t> > libRootcoin::listaddressgroupings() {
	string command = "listaddressgroupings";
	Value params, result;
	vector< vector<addressgrouping_t> > ret;
	result = sendcommand(command, params);

	for(ValueIterator it1 = result.begin(); it1 != result.end(); it1++){
		Value val1 = (*it1);
		vector<addressgrouping_t> tmp1;

		for(ValueIterator it2 = val1.begin(); it2 != val1.end(); it2++){
				Value val2 = (*it2);
				addressgrouping_t tmp2;

				tmp2.address = val2.operator []((uint)0).asString();
				tmp2.balance = val2.operator []((uint)1).asDouble();
				tmp2.account = (val2.isValidIndex(2) ? val2.operator []((uint)2).asString() : "");
				tmp1.push_back(tmp2);
		}

		ret.push_back(tmp1);
	}

	return ret;
}

bool libRootcoin::move(const string& fromaccount, const string& toaccount, double amount, int minconf) {
	string command = "move";
	Value params, result;

	params.append(fromaccount);
	params.append(toaccount);
	params.append(amount);
	params.append(minconf);
	result = sendcommand(command, params);

	return result.asBool();
}

bool libRootcoin::move(const string& fromaccount, const string& toaccount, double amount, const string& comment, int minconf) {
	string command = "move";
	Value params, result;

	params.append(fromaccount);
	params.append(toaccount);
	params.append(amount);
	params.append(minconf);
	params.append(comment);
	result = sendcommand(command, params);

	return result.asBool();
}

void libRootcoin::setaccount(const string& rootcoinaddress, const string& account){
	string command = "setaccount";
	Value params;

	params.append(rootcoinaddress);
	params.append(account);

	sendcommand(command, params);
}

string libRootcoin::sendtoaddress(const string& rootcoinaddress, double amount) {
	string command = "sendtoaddress";
	Value params, result;

	params.append(rootcoinaddress);
	params.append(amount);

	result = sendcommand(command, params);
	return result.asString();
}

string libRootcoin::sendtoaddress(const string& rootcoinaddress, double amount, const string& comment, const string& comment_to) {
	string command = "sendtoaddress";
	Value params, result;

	params.append(rootcoinaddress);
	params.append(amount);
	params.append(comment);
	params.append(comment_to);

	result = sendcommand(command, params);
	return result.asString();
}

string libRootcoin::sendfrom(const string& fromaccount, const string& torootcoinaddress, double amount) {
	string command = "sendfrom";
	Value params, result;

	params.append(fromaccount);
	params.append(torootcoinaddress);
	params.append(amount);

	result = sendcommand(command, params);
	return result.asString();
}

string libRootcoin::sendfrom(const string& fromaccount, const string& torootcoinaddress, double amount, const string& comment, const string& comment_to, int minconf) {
	string command = "sendfrom";
	Value params, result;

	params.append(fromaccount);
	params.append(torootcoinaddress);
	params.append(amount);
	params.append(minconf);
	params.append(comment);
	params.append(comment_to);

	result = sendcommand(command, params);
	return result.asString();
}

string libRootcoin::sendmany(const string& fromaccount, const map<string,double>& amounts) {
	string command = "sendmany";
	Value params, result;

	params.append(fromaccount);

	Value obj(Json::objectValue);
	for(map<string,double>::const_iterator it = amounts.begin(); it != amounts.end(); it++){
		obj[(*it).first] = (*it).second;
	}

	params.append(obj);

	result = sendcommand(command, params);
	return result.asString();
}

string libRootcoin::sendmany(const string& fromaccount, const map<string,double>& amounts, const string comment, int minconf) {
	string command = "sendmany";
	Value params, result;

	params.append(fromaccount);

	Value obj(Json::objectValue);
	for(map<string,double>::const_iterator it = amounts.begin(); it != amounts.end(); it++){
		obj[(*it).first] = (*it).second;
	}

	params.append(obj);
	params.append(minconf);
	params.append(comment);

	result = sendcommand(command, params);
	return result.asString();
}

vector<unspenttxout_t> libRootcoin::listunspent(int minconf, int maxconf) {
	string command = "listunspent";
	Value params, result;
	vector<unspenttxout_t> ret;

	params.append(minconf);
	params.append(maxconf);
	result = sendcommand(command, params);

	for(ValueIterator it = result.begin(); it != result.end(); it++){
		Value val = (*it);
		unspenttxout_t tmp;

		tmp.txid = val["txid"].asString();
		tmp.n = val["vout"].asUInt();
		tmp.address = val["address"].asString();
		tmp.account = val["account"].asString();
		tmp.scriptPubKey = val["scriptPubKey"].asString();
		tmp.amount = val["amount"].asDouble();
		tmp.confirmations = val["confirmations"].asInt();

		ret.push_back(tmp);
	}

	return ret;
}

vector<txout_t> libRootcoin::listlockunspent() {
	string command = "listlockunspent";
	Value params, result;
	vector<txout_t> ret;
	result = sendcommand(command, params);

	for(ValueIterator it = result.begin(); it != result.end(); it++){
		Value val = (*it);
		txout_t tmp;

		tmp.txid = val["txid"].asString();
		tmp.n = val["vout"].asUInt();
		ret.push_back(tmp);
	}

	return ret;
}

bool libRootcoin::lockunspent(bool unlock, const vector<txout_t>& outputs) {
	string command = "lockunspent";
	Value params, result;

	Value vec(Json::arrayValue);
	for(vector<txout_t>::const_iterator it = outputs.begin(); it != outputs.end(); it++){
		Value val;
		txout_t tmp = (*it);

		val["txid"] = tmp.txid;
		val["vout"] = tmp.n;
		vec.append(val);
	}

	params.append(unlock);
	params.append(vec);
	result = sendcommand(command, params);

	return result.asBool();
}

/* === Mining functions === */
string libRootcoin::getbestblockhash() {
	string command = "getbestblockhash";
	Value params, result;
	result = sendcommand(command, params);

	return result.asString();
}

string libRootcoin::getblockhash(int blocknumber) {
	string command = "getblockhash";
	Value params, result;
	params.append(blocknumber);
	result = sendcommand(command, params);

	return result.asString();
}

blockinfo_t libRootcoin::getblock(const string& blockhash) {
	string command = "getblock";
	Value params, result;
	blockinfo_t ret;

	params.append(blockhash);
	result = sendcommand(command, params);

	ret.hash = result["hash"].asString();
	ret.confirmations = result["confirmations"].asInt();
	ret.size = result["size"].asInt();
	ret.height = result["height"].asInt();
	ret.version = result["version"].asInt();
	ret.merkleroot = result["merkleroot"].asString();

	for(ValueIterator it = result["tx"].begin(); it != result["tx"].end(); it++){
		ret.tx.push_back((*it).asString());
	}

	ret.time = result["time"].asUInt();
	ret.nonce = result["nonce"].asUInt();
	ret.bits = result["bits"].asString();
	ret.difficulty = result["difficulty"].asDouble();
	ret.chainwork = result["chainwork"].asString();
	ret.previousblockhash = result["previousblockhash"].asString();
	ret.nextblockhash = result["nextblockhash"].asString();

	return ret;
}

int libRootcoin::getblockcount() {
	string command = "getblockcount";
	Value params, result;
	result = sendcommand(command, params);

	return result.asInt();
}

void libRootcoin::setgenerate(bool generate, int genproclimit) {
	string command = "setgenerate";
	Value params;
	params.append(generate);
	params.append(genproclimit);
	sendcommand(command, params);
}

bool libRootcoin::getgenerate() {
	string command = "getgenerate";
	Value params, result;
	result = sendcommand(command, params);

	return result.asBool();
}

double libRootcoin::getdifficulty() {
	string command = "getdifficulty";
	Value params, result;
	result = sendcommand(command, params);

	return result.asDouble();
}

int libRootcoin::gethashespersec() {
	string command = "gethashespersec";
	Value params, result;
	result = sendcommand(command, params);

	return result.asInt();
}

mininginfo_t libRootcoin::getmininginfo() {
	string command = "getmininginfo";
	Value params, result;
	mininginfo_t ret;

	result = sendcommand(command, params);

	ret.blocks = result["blocks"].asInt();
	ret.currentblocksize = result["currentblocksize"].asInt();
	ret.currentblocktx = result["currentblocktx"].asInt();
	ret.difficulty = result["difficulty"].asDouble();
	ret.errors = result["errors"].asString();
	ret.genproclimit = result["genproclimit"].asInt();
	ret.networkhashps = result["networkhashps"].asDouble();
	ret.pooledtx = result["pooledtx"].asInt();
	ret.testnet = result["testnet"].asBool();
	ret.generate = result["generate"].asBool();
	ret.hashespersec = result["hashespersec"].asInt();

	return ret;
}

workdata_t libRootcoin::getwork() {
	string command = "getwork";
	Value params, result;
	workdata_t ret;

	result = sendcommand(command, params);

	ret.midstate = result["midstate"].asString();
	ret.data = result["data"].asString();
	ret.hash1 = result["hash1"].asString();
	ret.target = result["target"].asString();

	return ret;
}

bool libRootcoin::getwork(const string& data) {
	string command = "getwork";
	Value params, result;

	params.append(data);
	result = sendcommand(command, params);

	return result.asBool();
}

txsinceblock_t libRootcoin::listsinceblock(const string& blockhash, int target_confirmations) {
	string command = "listsinceblock";
	Value params, result;
	txsinceblock_t ret;

	params.append(blockhash);
	params.append(target_confirmations);
	result = sendcommand(command, params);

	for(ValueIterator it = result["transactions"].begin(); it != result["transactions"].end(); it++){
		Value val = (*it);
		transactioninfo_t tmp;

		tmp.account = val["account"].asString();
		tmp.address = val["address"].asString();
		tmp.category = val["category"].asString();
		tmp.amount = val["amount"].asDouble();
		tmp.confirmations = val["confirmations"].asInt();
		tmp.blockhash = val["blockhash"].asString();
		tmp.blockindex = val["blockindex"].asInt();
		tmp.blocktime = val["blocktime"].asInt();
		tmp.txid = val["txid"].asString();

		for (ValueIterator it2 = val["walletconflicts"].begin();
				it2 != val["walletconflicts"].end(); it2++) {
			tmp.walletconflicts.push_back((*it2).asString());
		}

		tmp.time = val["time"].asInt();
		tmp.timereceived = val["timereceived"].asInt();

		ret.transactions.push_back(tmp);
	}

	ret.lastblock = result["lastblock"].asString();

	return ret;
}


/* === Raw transaction calls === */
getrawtransaction_t libRootcoin::getrawtransaction(const string& txid, int verbose) {
	string command = "getrawtransaction";
	Value params, result;
	getrawtransaction_t ret;

	params.append(txid);
	params.append(verbose);
	result = sendcommand(command, params);

	ret.hex = ((verbose == 0) ? result.asString() : result["hex"].asString());

	if(verbose != 0){
		ret.txid = result["txid"].asString();
		ret.version = result["version"].asInt();
		ret.locktime = result["locktime"].asInt();
		for (ValueIterator it = result["vin"].begin(); it != result["vin"].end();
				it++) {
			Value val = (*it);
			vin_t input;
			input.txid = val["txid"].asString();
			input.n = val["vout"].asUInt();
			input.scriptSig.assm = val["scriptSig"]["asm"].asString();
			input.scriptSig.hex = val["scriptSig"]["hex"].asString();
			input.sequence = val["sequence"].asUInt();
			ret.vin.push_back(input);
		}

		for (ValueIterator it = result["vout"].begin(); it != result["vout"].end();
				it++) {
			Value val = (*it);
			vout_t output;

			output.value = val["value"].asDouble();
			output.n = val["n"].asUInt();
			output.scriptPubKey.assm = val["scriptPubKey"]["asm"].asString();
			output.scriptPubKey.hex = val["scriptPubKey"]["hex"].asString();
			output.scriptPubKey.reqSigs = val["scriptPubKey"]["reqSigs"].asInt();

			output.scriptPubKey.type = val["scriptPubKey"]["type"].asString();
			for(ValueIterator it2 = val["scriptPubKey"]["addresses"].begin(); it2 != val["scriptPubKey"]["addresses"].end(); it2++){
				output.scriptPubKey.addresses.push_back((*it2).asString());
			}

			ret.vout.push_back(output);
		}
		ret.blockhash = result["blockhash"].asString();
		ret.confirmations = result["confirmations"].asUInt();
		ret.time = result["time"].asUInt();
		ret.blocktime = result["blocktime"].asUInt();
	}

	return ret;
}

decoderawtransaction_t libRootcoin::decoderawtransaction(const string& hexString) {
	string command = "decoderawtransaction";
	Value params, result;
	decoderawtransaction_t ret;

	params.append(hexString);
	result = sendcommand(command, params);

	ret.txid = result["txid"].asString();
	ret.version = result["version"].asInt();
	ret.locktime = result["locktime"].asInt();
	for (ValueIterator it = result["vin"].begin(); it != result["vin"].end();
			it++) {
		Value val = (*it);
		vin_t input;
		input.txid = val["txid"].asString();
		input.n = val["vout"].asUInt();
		input.scriptSig.assm = val["scriptSig"]["asm"].asString();
		input.scriptSig.hex = val["scriptSig"]["hex"].asString();
		input.sequence = val["sequence"].asUInt();
		ret.vin.push_back(input);
	}

	for (ValueIterator it = result["vout"].begin(); it != result["vout"].end();
			it++) {
		Value val = (*it);
		vout_t output;

		output.value = val["value"].asDouble();
		output.n = val["n"].asUInt();
		output.scriptPubKey.assm = val["scriptPubKey"]["asm"].asString();
		output.scriptPubKey.hex = val["scriptPubKey"]["hex"].asString();
		output.scriptPubKey.reqSigs = val["scriptPubKey"]["reqSigs"].asInt();

		output.scriptPubKey.type = val["scriptPubKey"]["type"].asString();
		for(ValueIterator it2 = val["scriptPubKey"]["addresses"].begin(); it2 != val["scriptPubKey"]["addresses"].end(); it2++){
			output.scriptPubKey.addresses.push_back((*it2).asString());
		}

		ret.vout.push_back(output);
	}

	return ret;
}

string libRootcoin::sendrawtransaction(const string& hexString) {
	string command = "sendrawtransaction";
	Value params, result;
	params.append(hexString);
	result = sendcommand(command, params);

	return result.asString();
}


string libRootcoin::createrawtransaction(const vector<txout_t>& inputs, const map<string,double>& amounts) {
	string command = "createrawtransaction";
	Value params, result;

	Value vec(Json::arrayValue);
	for(vector<txout_t>::const_iterator it = inputs.begin(); it != inputs.end(); it++){
		Value val;
		txout_t tmp = (*it);

		val["txid"] = tmp.txid;
		val["vout"] = tmp.n;

		vec.append(val);
	}

	Value obj(Json::objectValue);
	for(map<string,double>::const_iterator it = amounts.begin(); it != amounts.end(); it++){
		obj[(*it).first] = (*it).second;
	}

	params.append(vec);
	params.append(obj);
	result = sendcommand(command, params);

	return result.asString();
}

signrawtransaction_t libRootcoin::signrawtransaction(const string& rawTx, const vector<signrawtxin_t> inputs) {
	string command = "signrawtransaction";
	Value params, result;
	signrawtransaction_t ret;

	params.append(rawTx);
	Value vec(Json::arrayValue);
	for(vector<signrawtxin_t>::const_iterator it = inputs.begin(); it != inputs.end(); it++){
		Value val;
		signrawtxin_t tmp = (*it);
		val["txid"] = tmp.txid;
		val["vout"] = tmp.n;
		val["scriptPubKey"] = tmp.scriptPubKey;
		if(tmp.redeemScript != ""){
			val["redeemScript"] = tmp.redeemScript;
		}
		vec.append(val);
	}

	params.append(vec);
	result = sendcommand(command, params);

	ret.hex = result["hex"].asString();
	ret.complete = result["complete"].asBool();

	return ret;
}

signrawtransaction_t libRootcoin::signrawtransaction(const string& rawTx, const vector<signrawtxin_t> inputs, const vector<string>& privkeys, const string& sighashtype) {
	string command = "signrawtransaction";
	Value params, result;
	signrawtransaction_t ret;

	params.append(rawTx);
	Value vec1(Json::arrayValue);
	for(vector<signrawtxin_t>::const_iterator it = inputs.begin(); it != inputs.end(); it++){
		Value val;
		signrawtxin_t tmp = (*it);
		val["txid"] = tmp.txid;
		val["vout"] = tmp.n;
		val["scriptPubKey"] = tmp.scriptPubKey;
		if(tmp.redeemScript != ""){
			val["redeemScript"] = tmp.redeemScript;
		}
		vec1.append(val);
	}

	Value vec2(Json::arrayValue);
	for(vector<string>::const_iterator it = privkeys.begin(); it != privkeys.end(); it++){
		Value val;
		vec2.append((*it));
	}

	params.append(vec1);
	params.append(vec2);
	params.append(sighashtype);
	result = sendcommand(command, params);

	ret.hex = result["hex"].asString();
	ret.complete = result["complete"].asBool();

	return ret;
}

vector<string> libRootcoin::getrawmempool() {
	string command = "getrawmempool";
	Value params, result;
	vector<string> ret;

	result = sendcommand(command, params);

	for(ValueIterator it = result.begin(); it != result.end(); it++){
		ret.push_back((*it).asString());
	}

	return ret;
}

string libRootcoin::getrawchangeaddress() {
	string command = "getrawchangeaddress";
	Value params, result;
	result = sendcommand(command, params);

	return result.asString();
}

utxoinfo_t libRootcoin::gettxout(const std::string& txid, int n, bool includemempool) {
	string command = "gettxout";
	Value params, result;
	utxoinfo_t ret;

	params.append(txid);
	params.append(n);
	params.append(includemempool);
	result = sendcommand(command, params);

	ret.bestblock = result["bestblock"].asString();
	ret.confirmations = result["confirmations"].asInt();
	ret.value = result["value"].asDouble();

	ret.scriptPubKey.assm = result["scriptPubKey"]["asm"].asString();
	ret.scriptPubKey.hex = result["scriptPubKey"]["hex"].asString();
	ret.scriptPubKey.reqSigs = result["scriptPubKey"]["reqSigs"].asInt();
	ret.scriptPubKey.type = result["scriptPubKey"]["type"].asString();
	for(ValueIterator it = result["scriptPubKey"]["addresses"].begin(); it != result["scriptPubKey"]["addresses"].end(); it++){
		ret.scriptPubKey.addresses.push_back((*it).asString());
	}

	ret.version = result["version"].asInt();
	ret.coinbase = result["coinbase"].asBool();

	return ret;
}

utxosetinfo_t libRootcoin::gettxoutsetinfo() {
	string command = "gettxoutsetinfo";
	Value params, result;
	utxosetinfo_t ret;
	result = sendcommand(command, params);

	ret.height = result["height"].asInt();
	ret.bestblock = result["bestblock"].asString();
	ret.transactions = result["transactions"].asInt();
	ret.txouts = result["txouts"].asInt();
	ret.bytes_serialized = result["bytes_serialized"].asInt();
	ret.hash_serialized = result["hash_serialized"].asString();
	ret.total_amount = result["total_amount"].asDouble();

	return ret;
}
