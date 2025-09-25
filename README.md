# elrs2hbridge

## 💡 Conceito

- Temos um receptor de rádio controle ELRS modelo EP2 que utiliza o protocolo CRSF;
- Temos um AtTiny84 que recebe os dados seriais do receptor e gera sinais de saída PWM;
- Temos uma ponte H LB1836 que recebe os sinais de saída do AtTiny84;
- Em um dos canais da ponte H é conectada uma bobina de eletroíma, que controla a direção de um carrinho RC da seguinte maneira:
  - Com a bobina desligada, a direção retorna naturalmente para o centro;
  - Com a bobina energizada em uma polaridade, a direção vira para um lado;
  - Invertendo a polaridade da bobina, a direção vira para o outro lado;
  - Variando o duty cycle do PWM é possível controlar o quanto a direção é virada para cada lado;
- No outro canal da ponte H, é ligado o motor principal do carrinho RC, que é controlado da seguinte maneira:
  - Com o motor parado, o carro não se move;
  - Com o motor energizado, o carrinho se move para frente;
  - Invertendo a polaridade do motor, o carrinho se move para trás;
  - Variando o duty cycle do PWM é possível controlar a velocidade do carrinho.

## 🏗️ Requisitos

🚀 Implementação **real de UART RX no ATtiny84 usando USI**, adaptada para **420 000 baud** (420 kbps, CRSF).

> ⚠️ Observação: 420 kbps exige **clock externo estável (16 ou 20 MHz cristal/resonador)**. Se usar o oscilador interno, o erro de baud pode atrapalhar.

---

## 📂 Estrutura

```css
/elrs2hbridge
 ├── .vscode/
 │    └── tasks.json
 ├── CMakeLists.txt
 ├── Makefile
 └── src/
      ├── main.c
      ├── usi_uart.c
      └── usi_uart.h
```

* `main.c` → lógica CRSF + controle PWM/DIR.
* `usi_uart.h` → cabeçalho da UART.
* `usi_uart.c` → Implementação **bit-bang UART RX** usando Timer1 + interrupção no ATtiny84.
Funciona em **420 kbps**, **8N1**.

---

## ✅ Resultado

Esse código:

* Compila em **AVR-GCC** para ATtiny84.
* Recebe **CRSF a 420 kbps** no pino **PA0**.
* Decodifica pacotes de canais (`0x16`).
* Usa **canal 0** como entrada.
* Controla a ponte **LB1836**:

  * `PA6` = DIR
  * `PA7` = PWM (duty de 0–255).

---

## 🔹 Fuses explicados

**Fuses recomendados** para rodar o ATtiny84 a **16 MHz cristal externo** (necessário pro CRSF a 420 kbaud rodar estável):

* **Low Fuse = `0xFF`**

  * `CKSEL=1111` → cristal full swing, 8–MHz+
  * `SUT=11` → startup 16k + 65ms
  * `CKDIV8=1` → desativado (não divide por 8)

* **High Fuse = `0xDF`**

  * `RSTDISBL=1` → mantém pino RESET ativo
  * `DWEN=1` → debugWIRE desativado
  * `SPIEN=0` → ISP habilitado (necessário pro USBasp)
  * `EESAVE=1` → EEPROM não apagada em regravação
  * `BODLEVEL=111` → brown-out desativado

* **Extended Fuse = `0xFF`**

  * Brown-out desabilitado (pode ajustar p/ `0xFD` → 2.7 V, se quiser mais robustez)

---

## 🔹 Como usar

```bash
make           # compila
make flash     # grava firmware via USBasp
make fuses     # configura fuses para 16 MHz cristal externo
```

---
