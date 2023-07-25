FROM confluentinc/cp-server-connect-base:7.3.3

RUN confluent-hub install --no-prompt --verbose mongodb/kafka-connect-mongodb:latest

ENV CONNECT_BOOTSTRAP_SERVERS="kafka-0.kafka-headless.default.svc.cluster.local:9092,kafka-1.kafka-headless.default.svc.cluster.local:9092,kafka-2.kafka-headless.default.svc.cluster.local:9092" \
    CONNECT_REST_PORT="8083" \
    CONNECT_GROUP_ID="kafka-connect" \
    CONNECT_CONFIG_STORAGE_TOPIC="_connect-configs" \
    CONNECT_OFFSET_STORAGE_TOPIC="_connect-offsets" \
    CONNECT_STATUS_STORAGE_TOPIC="_connect-status" \
    CONNECT_KEY_CONVERTER="io.confluent.connect.avro.AvroConverter" \
    CONNECT_KEY_CONVERTER_SCHEMA_REGISTRY_URL="http://schema-registry:8081" \
    CONNECT_VALUE_CONVERTER="io.confluent.connect.avro.AvroConverter" \
    CONNECT_VALUE_CONVERTER_SCHEMA_REGISTRY_URL="http://schema-registry:8081" \
    CONNECT_REST_ADVERTISED_HOST_NAME="kafka-connect" \
    CONNECT_LOG4J_APPENDER_STDOUT_LAYOUT_CONVERSIONPATTERN="[%d] %p %X{connector.context}%m (%c:%L)%n" \
    CONNECT_CONFIG_STORAGE_REPLICATION_FACTOR="3" \
    CONNECT_OFFSET_STORAGE_REPLICATION_FACTOR="3" \
    CONNECT_STATUS_STORAGE_REPLICATION_FACTOR="3" \
    CONNECT_AUTO_CREATE_TOPICS_ENABLE="true" \
    CONNECT_METADATA_MAX_AGE_MS="180000" \
    CONNECT_CONNECTIONS_MAX_IDLE_MS="180000" \
    CONNECT_OFFSET_FLUSH_INTERVAL_MS="10000" \
    CONNECT_PLUGIN_PATH="/usr/share/java,/usr/share/confluent-hub-components"

EXPOSE 8083

COPY sink.mongodb.json sink.mongodb.json
COPY init.sh init.sh

# Run the script on startup
CMD /bin/sh -c "./init.sh && tail -f /dev/null"