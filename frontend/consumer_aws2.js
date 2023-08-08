const { Kafka } = require('kafkajs');
const { WebSocketServer } = require("ws");
const Jimp = require('jimp');

const server = new WebSocketServer({ port: 5173 });

const kafkaBrokers = '13.57.244.86:32400';

const kafka = new Kafka({
  brokers: [kafkaBrokers],
});
const consumer = kafka.consumer({ groupId: 'bootstrap.servers' });
let running = true;

const stopRunning = () => {
  running = false;
  consumer.disconnect();
};

const consumeMessages = async (topic) => {
  await consumer.connect();
  await consumer.subscribe({ topic });

  await consumer.run({
    eachMessage: async ({ topic, partition, message }) => {
      try {
        const cameraIndex = parseInt(topic);
        const arr = message.value.toString().split(',');
        const l = arr.length;
        console.log("ARR length", l);
        if (l >= 1) {
          const [cls, confidence, encodedImage, timestamp] = arr;
          console.log('CLASS: ', cls);
          console.log('CONFIDENCE: ', confidence);
          console.log('TIMESTAMP: ', new Date(timestamp).toString());

          let img = async (encodeImg) => {
            const decodedImg = Buffer.from(encodeImg, 'base64');
            const resizedImage = await Jimp.read(decodedImg);
            return resizedImage;
          };

          try {
            const decodedImage = await img(encodedImage);
            console.log('Decoded image:', decodedImage);

            server.clients.forEach((socket) => {
              if (socket.readyState === WebSocketServer.OPEN) {
                socket.send(JSON.stringify({
                  'cls': cls,
                  'confidence': confidence.toString(),
                  'timestamp': new Date(timestamp).toString(),
                  'imageString': encodedImage
                }));
              }
            });
          } catch (error) {
            console.error('Error occurred while processing image:', error);
          }
        }
      } catch (error) {
        console.error(error);
      }
    },
  });
};

const main = async () => {
  if (process.argv.length !== 3) {
    console.log('Please provide the topic');
    process.exit(1);
  }

  process.on('SIGINT', stopRunning);

  const fileData = fs.readFileSync(process.argv[2], 'utf-8');
  const jsonData = JSON.parse(fileData);
  const cameras = jsonData.Camera_map;

  for (const cameraIndex in cameras) {
    await consumeMessages(cameraIndex);
  }
};

main().catch(console.error);
