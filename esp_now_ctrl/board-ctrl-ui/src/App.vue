<template>
  <div id="app">
    <h1>ESP32 LED Control</h1>
    <button @click="turnLEDOn">LED On</button>
    <button @click="turnLEDOff">LED Off</button>
  </div>
</template>

<script>
export default {
  name: 'App',
  methods: {
    async turnLEDOn() {
      try {
        await fetch('http://192.168.0.247/led/on'); // coordinator board에 신호 보냄
        console.log("LED ON")
      } catch (error) {
        console.error('Error:', error);
      }
    },
    async turnLEDOff() {
      try {
        await fetch('http://192.168.0.247/led/off');
        console.log("LED OFF")
      } catch (error) {
        console.error('Error:', error);
      }
    }
  }
}
</script>

<style scoped>
#app {
  text-align: center;
  margin-top: 60px;
}
button {
  margin: 10px;
  padding: 10px;
}
</style>
