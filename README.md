# house-temp
Full stack application that queries temperature sensors in my house

Use ESP2866 Wifi nodes with DHT11 temp sensors powered by coin batteries
These send data to a CloudAMQP instance that holds the data (make sure you're under a million monthly messages)
Write a cpp app that talks to Cloud AMQP (might not even need this react might be able to do it) and displays the most recent data
