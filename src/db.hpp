#ifndef MONGODB_HELPER_H
#define MONGODB_HELPER_H

#include <mongocxx/client.hpp>
#include <bsoncxx/builder/stream/document.hpp>
#include <bsoncxx/json.hpp>
#include <mongocxx/uri.hpp>
#include <mongocxx/instance.hpp>
#include <vector>
#include <string>
#include <iostream>

using bsoncxx::builder::basic::kvp;
using bsoncxx::builder::basic::make_array;
using bsoncxx::builder::basic::make_document;

class MongoDBHelper {
private:
    mongocxx::instance inst{};
    mongocxx::options::client client_options;
    mongocxx::uri mongoURI;
    mongocxx::client conn;
    std::string uri = "mongodb+srv://ovengurl:OmkarV23@cluster0.2ot3gkf.mongodb.net/?retryWrites=true&w=majority";

public:
    MongoDBHelper(){
        mongoURI = mongocxx::uri{ uri };
        auto api = mongocxx::options::server_api{ mongocxx::options::server_api::version::k_version_1 };
        client_options.server_api_opts(api);
        conn = mongocxx::client{ mongoURI, client_options};
    }

    std::vector<std::string> getDatabases() {
        return conn.list_database_names();
    }

    void insertDocument(const std::string& dbName, const std::string& collectionName, const bsoncxx::document::view& doc) {
        auto db = conn[dbName];
        auto collection = db[collectionName];
        collection.insert_one(doc);
    }
};

#endif // MONGODB_HELPER_H
