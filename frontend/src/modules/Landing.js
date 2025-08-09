// src/modules/Landing.js
import React, { useState } from 'react';
import Container from '@mui/material/Container';
import Typography from '@mui/material/Typography';

function Landing() {

    const[temperatureOut, setTemperatureOut] = useState('0.0')
    const[temperatureIsaac, setTemperatureIsaac] = useState('0.0')
    const[temperatureNiko, setTemperatureNiko] = useState('0.0')
    const[temperatureLiving, setTemperatureLiving] = useState('0.0')

    function getTemperature(){
        console.log("temperature")
        setTemperatureIsaac('0.0')
        setTemperatureOut('0.0')
        setTemperatureNiko('0.0')
        setTemperatureLiving('0.0')
    }

    return (
        <div>
            <Container maxWidth="sm">
                <Typography variant="h1" gutterBottom>
                    House Temp
                </Typography>
                <Typography variant="h2" gutterBottom>
                    Isaac's Room: {temperatureIsaac}
                </Typography>
                <Typography variant="h2" gutterBottom>
                    Niko's Room: {temperatureNiko}
                </Typography>
                <Typography variant="h2" gutterBottom>
                    Living Room: {temperatureLiving}
                </Typography>
                <Typography variant="h2" gutterBottom>
                    Outisde: {temperatureOut}
                </Typography>
            </Container>
        </div>
    );
}

export default Landing;