import socket
import signal
import sys
from datetime import datetime
from collections import deque

UDP_IP = "192.168.42.10"  # IP do PC (deve corresponder à configuração da placa)
UDP_PORT = 5000
VIBRATION_THRESHOLD = 5000  # Threshold para alertas
MAX_HISTORY = 100  # Manter histórico dos últimos 100 valores

# Estrutura para armazenar estatísticas
stats = {
    'total_readings': 0,
    'min_value': float('inf'),
    'max_value': 0,
    'sum_values': 0,
    'history': deque(maxlen=MAX_HISTORY),
    'high_vibration_events': 0,
    'last_timestamp': None,
    'sensor_id': None
}

def print_header():
    """Imprime header com informações da sessão"""
    print("\n" + "="*80)
    print(f"MONITOR DE VIBRAÇÃO - STM32MP1 + SW420")
    print(f"Servidor UDP escutando em {UDP_IP}:{UDP_PORT}")
    print(f"Limiar de alerta: {VIBRATION_THRESHOLD} ADC")
    print(f"Iniciado em: {datetime.now().strftime('%Y-%m-%d %H:%M:%S')}")
    print("="*80 + "\n")

def print_reading(sensor_id, timestamp, value, unit):
    """Formata e imprime uma leitura individual"""
    status = "⚠️  ALERTA" if int(value) > VIBRATION_THRESHOLD else "✓ Normal"
    print(f"[{timestamp}] {sensor_id:20s} | Valor: {value:>6s} {unit:>4s} | {status}")

def print_stats():
    """Imprime estatísticas agregadas"""
    if stats['total_readings'] == 0:
        return

    avg_value = stats['sum_values'] / stats['total_readings']

    print("\n" + "-"*80)
    print(f"ESTATÍSTICAS (últimas {min(stats['total_readings'], MAX_HISTORY)} leituras):")
    print(f"  Total de leituras:        {stats['total_readings']}")
    print(f"  Valor mínimo:             {stats['min_value']} ADC")
    print(f"  Valor máximo:             {stats['max_value']} ADC")
    print(f"  Valor médio:              {avg_value:.1f} ADC")
    print(f"  Eventos de alta vibração: {stats['high_vibration_events']}")
    if stats['last_timestamp']:
        print(f"  Última leitura:           {stats['last_timestamp']}")
    print("-"*80 + "\n")

def update_stats(value):
    """Atualiza estatísticas com novo valor"""
    value_int = int(value)

    stats['total_readings'] += 1
    stats['history'].append(value_int)
    stats['sum_values'] += value_int

    if value_int < stats['min_value']:
        stats['min_value'] = value_int

    if value_int > stats['max_value']:
        stats['max_value'] = value_int

    if value_int > VIBRATION_THRESHOLD:
        stats['high_vibration_events'] += 1

def parse_csv_message(message):
    """Parse da mensagem CSV e validação"""
    try:
        parts = message.strip().split(',')
        if len(parts) != 4:
            return None

        sensor_id, timestamp, value, unit = parts
        # Validação básica
        try:
            int(value)
        except ValueError:
            return None

        return {
            'sensor_id': sensor_id,
            'timestamp': timestamp,
            'value': value,
            'unit': unit
        }
    except Exception as e:
        print(f"❌ Erro ao fazer parse: {e}")
        return None

def signal_handler(sig, frame):
    """Tratamento para Ctrl+C gracioso"""
    print("\n" + "="*80)
    print("ENCERRANDO SERVIDOR...")
    print_stats()
    print("="*80)
    sock.close()
    sys.exit(0)

# Configura handler para Ctrl+C
signal.signal(signal.SIGINT, signal_handler)

# Cria socket
sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
sock.bind((UDP_IP, UDP_PORT))

print_header()

try:
    while True:
        data, addr = sock.recvfrom(1024)
        message = data.decode('utf-8')

        # Parse da mensagem
        parsed = parse_csv_message(message)
        if not parsed:
            print(f"❌ Mensagem inválida de {addr[0]}:{addr[1]} | Raw: {message}")
            continue

        # Atualiza sensor_id
        if stats['sensor_id'] is None:
            stats['sensor_id'] = parsed['sensor_id']
            print(f"✓ Sensor conectado: {stats['sensor_id']}")
            print(f"  IP: {addr[0]}, Porta: {addr[1]}\n")

        # Atualiza timestamp
        stats['last_timestamp'] = parsed['timestamp']

        # Imprime leitura
        print_reading(
            parsed['sensor_id'],
            parsed['timestamp'],
            parsed['value'],
            parsed['unit']
        )

        # Atualiza estatísticas
        update_stats(parsed['value'])

        # Imprime estatísticas a cada 20 leituras
        if stats['total_readings'] % 20 == 0:
            print_stats()

except KeyboardInterrupt:
    signal_handler(None, None)
except Exception as e:
    print(f"❌ Erro no servidor: {e}")
    sock.close()
    sys.exit(1)