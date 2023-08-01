FROM confluentinc/cp-kafka-connect:latest

RUN confluent-hub install --no-prompt mongodb/kafka-connect-mongodb:latest

RUN confluent-hub list

ENV CONNECT_PLUGIN_PATH="/usr/share/java,/usr/share/confluent-hub-components"

EXPOSE 8083

COPY worker.properties /etc/kafka-connect/worker.properties
COPY mongodb-sink.properties /etc/kafka-connect/mongodb-sink.properties
