FROM confluentinc/cp-kafka-connect:latest

RUN confluent-hub install --no-prompt mongodb/kafka-connect-mongodb:latest
RUN confluent-hub install --no-prompt confluentinc/connect-transforms:latest

RUN confluent-hub list

ENV CONNECT_BOOTSTRAP_SERVERS="kafka-0.kafka-headless.default.svc.cluster.local:9092,kafka-1.kafka-headless.default.svc.cluster.local:9092,kafka-2.kafka-headless.default.svc.cluster.local:9092" \
    CONNECT_REST_PORT="8083" \
    CONNECT_GROUP_ID="kafka-connect" \
    CONNECT_CONFIG_STORAGE_TOPIC="connect-configs" \
    CONNECT_OFFSET_STORAGE_TOPIC="connect-offsets" \
    CONNECT_STATUS_STORAGE_TOPIC="connect-status" \
    CONNECT_KEY_CONVERTER="org.apache.kafka.connect.storage.StringConverter" \
    CONNECT_VALUE_CONVERTER="org.apache.kafka.connect.json.JsonConverter" \
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

COPY worker.properties /etc/kafka-connect/worker.properties
COPY mongodb-sink.properties /etc/kafka-connect/mongodb-sink.properties

WORKDIR /etc/kafka-connect
CMD ["connect-distributed", "/etc/kafka-connect/worker.properties", "/etc/kafka-connect/mongodb-sink.properties"]
