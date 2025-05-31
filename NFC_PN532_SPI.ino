#include <SPI.h>
#include <Adafruit_PN532.h>

#define PN532_SS 5
Adafruit_PN532 nfc(PN532_SS);

void setup(void) {
  Serial.begin(115200);
  Serial.println("🚀 PN532 NFC URL 쓰기 시작");

  nfc.begin();

  uint32_t versiondata = nfc.getFirmwareVersion();
  if (!versiondata) {
    Serial.println("❌ PN532 인식 실패");
    while (1);
  }

  nfc.SAMConfig();
  Serial.println("✅ PN532 준비 완료! NTAG216 태그를 올려주세요.");
}

void loop(void) {
  if (nfc.inListPassiveTarget()) {
    Serial.println("📶 태그 감지됨. URL 쓰는 중...");

    // 정확히 맞는 길이: https://naver.com (12바이트)
    uint8_t ndef[20] = {
      0x03, 0x11,              // NDEF 메시지 총 길이 = 17
      0xD1, 0x01, 0x0D,        // NDEF 헤더
      0x55,                   // URI 타입
      0x03,                   // "https://"
      'n','a','v','e','r','.','c','o','m',
      0xFE, 0x00, 0x00, 0x00  // 종료 마커 + 패딩
    };

    // 페이지 4~8까지 총 5페이지에 걸쳐 쓰기 (20바이트)
    for (int i = 0; i < 5; i++) {
      nfc.ntag2xx_WritePage(4 + i, &ndef[i * 4]);
    }

    Serial.println("✅ URL 기록 완료! 아이폰으로 스캔해보세요.");
    delay(5000);
  }
}


/////////////////////////////////////////////////////////////////////////////////////
/*#include <Wire.h>
#include <SPI.h>
#include <Adafruit_PN532.h>

// PN532 모듈과 연결된 SS(또는 SDA) 핀 정의
#define PN532_SS 5
Adafruit_PN532 nfc(PN532_SS);

void setup(void) {
  Serial.begin(115200);
  Serial.println(F("🚀 PN532 NFC URL 쓰기 시작"));

  nfc.begin();
  uint32_t versiondata = nfc.getFirmwareVersion();
  if (!versiondata) {
    Serial.println(F("❌ PN532 인식 실패 (getFirmwareVersion)"));
    while (1);
  }

  // SAMConfig: PN532를 일반 NFC 리더/라이터 모드로 동작시키는 설정
  nfc.SAMConfig();
  Serial.println(F("✅ PN532 준비 완료! NTAG216 태그를 가까이 대세요."));
}

void loop(void) {
  // 태그가 가까이 오면 true 리턴 (Polling 모드)
  if (nfc.inListPassiveTarget()) {
    Serial.println(F("📶 NTAG216 태그 감지됨. URL 쓰는 중..."));

    // ────────── 수정된 NDEF + TLV 바이트 배열 ──────────
    // “https://naver.com”을 위한 올바른 NDEF 구조
    uint8_t ndef[20] = {
      // 페이지 4 (바이트 0~3)
      0x00,        // (패딩: 실제 TLV 시작 전 0x00로 채움)
      0x03, 0x0B,  // TLV Tag=0x03, Length=0x0B (11 = Type(1) + Payload(10))
      0xD1,        // Record Header

      // 페이지 5 (바이트 4~7)
      0x01,        // Type Length = 0x01 (“U” 한 글자)
      0x0A,        // Payload Length = 0x0A (10바이트 = 1(prefix) + 9(“naver.com”))
      0x55,        // Type = 0x55 (‘U’)
      0x03,        // URI Identifier = 0x03 (“https://”)

      // 페이지 6 (바이트 8~11)  “n”, “a”, “v”, “e”
      'n', 'a', 'v', 'e',

      // 페이지 7 (바이트 12~15) “r”, “.”, “c”, “o”
      'r', '.', 'c', 'o',

      // 페이지 8 (바이트 16~19)
      'm',        // “m”
      0xFE,       // NDEF Terminator(0xFE)
      0x00, 0x00 // 패딩 (남은 바이트는 0x00으로 채움)
    };
    // ────────────────────────────────────────────────────

    // 실제로 NTAG216의 페이지 4번부터 8번까지(5페이지 × 4바이트 = 20바이트)를 씀
    for (uint8_t page = 4; page <= 8; page++) {
      // 각 페이지마다 배열 안의 4바이트씩 꺼내서 쓰기
      bool ok = nfc.ntag2xx_WritePage(page, ndef + (page - 4) * 4);
      if (!ok) {
        Serial.print(F("❌ 페이지 "));
        Serial.print(page);
        Serial.println(F(" 쓰기 실패! 태그와 PN532 연결 상태를 확인하세요."));
        return; // 중단 (필요 시 재시도 로직을 넣어도 됩니다)
      }
      delay(5);  // 약간의 짧은 딜레이 (안정성 확보)
    }

    Serial.println(F("✅ URL 기록 완료! 스마트폰으로 다시 스캔해보세요."));
    delay(5000);
  }
}*/
