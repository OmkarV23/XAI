const { Kafka } = require('kafkajs');
const fs = require('fs');
const Jimp = require('jimp')
const { WebSocketServer } = require("ws");

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
        const arr = message.value.toString().split('\t');
        const l = arr.length
        console.log("ARR length", l)
        if (l >= 3) {
          const [cls, confidence, encodedImage, timestamp] = arr
          console.log('CLASS: ', cls)
          console.log('CONFIDENCE: ', confidence)
          console.log('TIMESTAMP: ', Date(timestamp).toString())
          let encodeImg = encodedImage.toString()
          console.log('ENCODED IMAGE: ', encodeImg.slice(-10,))

          //outputing the image in local directory
          let img = async (encodeImg) => {
            // Decode the image from base64
            const decodedImg = Buffer.from(encodeImg, 'base64')
            Jimp.read(decodedImg, (err, res) => {
              if (err) throw new Error(err);
              res.quality(100).write("resized_5.jpg")
            });
          };

          // function call to encode image
          img(encodeImg).then((decodedImage) => {
            console.log('Resolved image:', decodedImage);
          })
            .catch((error) => {
              console.error('Error occurred:', error);
            });

          // send a message to the client
          server.on("connection", (socket) => {
            socket.send(JSON.stringify({
              'cls': parseInt(cls.slice(2), 10),
              'confidence': confidence.toString(),
              'timestamp': Date(timestamp).toString(),
              'imageString': encodeImg
            }));
          });

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

  const fileData = fs.readFileSync(process.argv[2], 'utf-8')
  const jsonData = JSON.parse(fileData)
  const cameras = jsonData.Camera_map

  for (const cameraIndex in cameras) {
    await consumeMessages(cameraIndex)
  }
};

main().catch(console.error);