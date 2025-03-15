#include <Arduino.h>
#include "driver/i2s.h"

const i2s_port_t I2S_PORT = I2S_NUM_0;

void setup()
{
  Serial.begin(115200);
  esp_err_t err;

  // I2S Configuration
  const i2s_config_t i2s_config = {
      .mode = i2s_mode_t(I2S_MODE_MASTER | I2S_MODE_RX), // Receive, not transfer
      .sample_rate = 16000,                             // Test with 16kHz
      .bits_per_sample = I2S_BITS_PER_SAMPLE_16BIT,     // 16-bit samples
      .channel_format = I2S_CHANNEL_FMT_ONLY_LEFT,      // Use left channel
      .communication_format = I2S_COMM_FORMAT_STAND_I2S,
      .intr_alloc_flags = ESP_INTR_FLAG_LEVEL1,         // Interrupt level 1
      .dma_buf_count = 4,                               // Increased buffers
      .dma_buf_len = 64                                 // Increased buffer length
  };

  // I2S Pin Configuration
  const i2s_pin_config_t pin_config = {
      .bck_io_num = 18,                   // MIC_SCK (Clock)
      .ws_io_num = 20,                    // MIC_WS (Word Select)
      .data_out_num = I2S_PIN_NO_CHANGE,  // Not used for input
      .data_in_num = 19                   // MIC_SD (Data)
  };

  // Install I2S driver
  err = i2s_driver_install(I2S_PORT, &i2s_config, 0, NULL);
  if (err != ESP_OK)
  {
    Serial.printf("Failed installing driver: %d\n", err);
    while (true)
      ;
  }
  else
  {
    Serial.println("I2S driver installed.");
  }

  // Set I2S pin configuration
  err = i2s_set_pin(I2S_PORT, &pin_config);
  if (err != ESP_OK)
  {
    Serial.printf("Failed setting pin: %d\n", err);
    while (true)
      ;
  }
  else
  {
    Serial.println("I2S pin configuration successful.");
  }

  // Start with zeroing the input buffer
  i2s_zero_dma_buffer(I2S_PORT);
}

int16_t sBuffer[64]; // Adjust buffer size as needed

void loop()
{
  size_t bytesIn = 0;

  // Read data from I2S
  esp_err_t result = i2s_read(I2S_PORT, sBuffer, sizeof(sBuffer), &bytesIn, portMAX_DELAY);

  if (result == ESP_OK)
  {
    // Print the number of bytes read
    Serial.printf("Bytes read: %d\n", bytesIn);

    // Check for valid data
    if (bytesIn > 0)
    {
      for (int i = 0; i < bytesIn / 2; i++) // Divide by 2 because we have 16-bit samples
      {
        Serial.print(sBuffer[i]);
        Serial.print(" ");
      }
      Serial.println();
    }
    else
    {
      Serial.println("No data received.");
    }
  }
  else
  {
    // Log error in case of read failure
    Serial.printf("i2s_read failed: %d\n", result);
  }

  delay(100); // Add some delay to prevent flooding the serial output
}
