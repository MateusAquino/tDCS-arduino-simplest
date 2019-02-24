# tDCS_arduino_simplest (pt-BR) v2.0m
## tDCS_arduino_simples

Para fazer este tDCS você precisará apenas de um arduino, resistor, capacitor e alguns cabos

![Assembled](/imgs/wtf.jpg?raw "Assembled")

### Componentes:
1) Arduino
   - Pino D13 como saída PWM (**pode** ser alterado).
   - Pino A0 como entrada analógica (para feedback de corrente).
   - Pino GND apenas para GND.
2) R - Resistor (Uso de 470 Ω, mas provavelmente entre 300-1000 Ω funciona, **você precisará alterar no código fonte**)
3) C - capacitor (220 μF). Serve para estabilizar os pulsos do PWM.
4) Eletrodos de Esponja (Use água salina para molhá-lo).

## Como funciona
O Arduino calcula a corrente padrão (pode ser alterado) que passará pelo seu cérebro apenas por mudar a voltagem de saída. Você também pode alterar o valor do target_mA pelo serial CLI (Console).

## Caso queira testar (Leia atentamente)
0) Você deve ler mais sobre tDCS primeiro. Não é aprovado pelo FDA e pode ser prejudicial. 
1) Monte o circuito (abaixo).
2) Instale o código no seu arduino e altere as configurações e parametros na área de `//HARDWARE PARAMS` e `//CONFIGURABLE PARAMS`.
3) Lembre-se de alterar o boud rate do Serial para: **115200** (para executar comandos, troque o *No Line Ending* para *Carriage Return*).
3) Para melhor acompanhamento e segurança, utilize a ferramenta PuTTY, e defina no código fonte: **`putty = true`**.
4) Caso queira ver em forma de gráficos, altere no código fonte: **`plotter = true`** e entre no modo Serial Plotter do Arduino.

## Serial Plotter

![Serial Plotter](/imgs/Plotter.png?raw=true "Serial Plotter")
Dados:
   - 1ª (azul) - corrente máxima (target_mA)
   - 2ª (vermelho) - corrente mínima (0 mA)
   - 3ª (verde) - corrente passando pelo seu cérebro (smoothered_mA)   

## PuTTY

![PuTTY](/imgs/putty.png?raw=true "Serial Plotter")

Recomendações de definições:

* Window
    * 61 Colunas e 20 Linhas
    * *Display scrollbar* desativado
* Window > Appearance
    * Fonte: Lucida Console, 28px

## Circuito

![Schematic](/imgs/Sketch_bb.png?raw=true "Schematic")


## Doações para o [autor](https://github.com/e-kolkhoz/tDCS_arduino_simplest) original

Paypal:
https://www.paypal.me/ekolkhoz

Bitcoin: 
1PBiNy33ZqRyAA8HcSgq6GS4f6PH5R9zUu

![btc](/imgs/c1av9wf.png?raw=true "btc")
