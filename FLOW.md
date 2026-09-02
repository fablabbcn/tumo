# Como os programas funcionam

Resumo em pseudocódigo (linguagem do dia a dia) e um diagrama simples para cada
sketch. Um programa de Arduino tem duas partes:

- **Preparar** — acontece **uma única vez**, quando a placa liga.
- **Repetir** — acontece **sem parar**, muitas vezes por segundo, até desligar.

---

## boia — `buoy.ino`

Mede o que o mar está fazendo (luz, temperatura, pressão e balanço) e envia os
números para o computador.

### Pseudocódigo

```
PREPARAR (uma vez):
    ligar a conversa com o computador
    ligar os sensores (luz, barômetro, acelerômetro)
    se algum sensor não responder: piscar o LED para sempre e parar
    escrever os nomes das colunas

REPETIR PARA SEMPRE:
    esperar um pouco
    medir a luz
    medir a temperatura e a pressão
    medir o balanço (acelerômetro)
    enviar os números para o computador
    piscar o LED uma vez
```

### Diagrama

```mermaid
flowchart TD
    A([Início]) --> B["Preparar: ligar sensores"]
    B --> C["Esperar um pouco"]
    C --> D["Medir luz, temperatura,<br/>pressão e balanço"]
    D --> E["Enviar os números<br/>para o computador"]
    E --> F["Piscar o LED"]
    F --> C
```

---

## vaso — `planter.ino`

Mede a umidade da terra e a temperatura + umidade do ar, mostra tudo na tela e
liga a bomba de água quando a terra está seca.

### Pseudocódigo

```
PREPARAR (uma vez):
    ligar a conversa com o computador
    desligar a bomba
    ligar a tela (OLED)
    ligar o sensor de ar (temperatura + umidade)
    escrever os nomes das colunas

REPETIR PARA SEMPRE:
    esperar 1 segundo
    medir a umidade da terra
    transformar a medida em porcentagem (0 a 100)
    medir a temperatura e a umidade do ar
    SE a terra estiver seca (umidade da terra menor que 50%):
        ligar a bomba
    SENÃO:
        desligar a bomba
    enviar os números para o computador
    mostrar na tela: umidade da terra, temperatura e umidade do ar
```

### Diagrama

```mermaid
flowchart TD
    A([Início]) --> B["Preparar: bomba desligada,<br/>ligar a tela e o sensor de ar"]
    B --> C["Esperar 1 segundo"]
    C --> D["Medir a umidade da terra"]
    D --> E["Transformar em porcentagem<br/>(0 a 100)"]
    E --> F["Medir a temperatura<br/>e a umidade do ar"]
    F --> G{"A terra está seca?<br/>umidade menor que 50%"}
    G -- "Sim" --> H["Ligar a bomba"]
    G -- "Não" --> I["Desligar a bomba"]
    H --> J["Mostrar na tela e enviar ao computador:<br/>umidade da terra, temperatura<br/>e umidade do ar"]
    I --> J
    J --> C
```

## Fazer PNGs

npx -p @mermaid-js/mermaid-cli mmdc -i FLOW.md -o FLOW.png
