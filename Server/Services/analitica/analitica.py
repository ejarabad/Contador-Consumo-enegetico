import paho.mqtt.client as mqtt
import time
import csv
import pandas as pd
from sklearn.linear_model import LinearRegression
import numpy as np
import smtplib
from sklearn.model_selection import train_test_split
from sklearn.metrics import mean_squared_error, r2_score
from influxdb_client.client.write_api import SYNCHRONOUS
import os; import locale;  os.environ["PYTHONIOENCODING"] = "utf-8";
myLocale=locale.setlocale(category=locale.LC_ALL, locale="en_GB.UTF-8");
from influxdb_client import InfluxDBClient, Point, WritePrecision

# Configura los parámetros del correo electrónico
remitente = 'lasama1525@gmail.com'
password = 'znxtmpcebromjqga'
destinatario = 'lasama1525@gmail.com'
asunto = 'Prueba'
cuerpo = 'Cuerpo del correo electrónico'

# Configura el servidor SMTP y el puerto a utilizar
server = smtplib.SMTP('smtp.gmail.com', 587)
server.starttls()

# Inicia sesión en el servidor SMTP
server.login(remitente, password)

token = "token-secreto"
org = "org"
bucket = "RabbitBucket"
client1 = InfluxDBClient(url="http://4.204.226.150:8086", token=token)
pot = []



# Define las variables que se usarán para guardar los mensajes
mensaje_1 = '' #Valor voltaje
mensaje_2 = '' #Valor Corriente
mensaje_3 = 0 #Valor potencia
costo = 800
datos = []
promPot = 0
tiempo_inicial = time.time() # guarda el tiempo actual

# Abre el archivo CSV en modo de escritura y define el encabezado
with open('datos.csv', 'w', newline='') as archivo_csv:
    writer = csv.writer(archivo_csv)
    writer.writerow(['promedioPotencia', 'costo', 'costoEnergia'])

# Define la función que se llamará cada vez que se reciba un mensaje MQTT
def on_message(client, userdata, message):
    global mensaje_1, mensaje_2, mensaje_3
    # Recupera el mensaje y lo guarda en la variable correspondiente
    if message.topic == 'voltaje':
        mensaje_1 = str(message.payload.decode('utf-8'))
    elif message.topic == 'corriente':
        mensaje_2 = str(message.payload.decode('utf-8'))
    elif message.topic == 'potencia':
        mensaje_3 = str(message.payload.decode('utf-8'))

# Configura el cliente MQTT y se conecta al broker de RabbitMQ
client = mqtt.Client()
client.connect('4.204.226.150', 1883)

# Configura la función que se llamará cada vez que se reciba un mensaje
client.on_message = on_message

# Se suscribe a los topics de interés
client.subscribe('voltaje')
client.subscribe('corriente')
client.subscribe('potencia')

# Inicia el loop del cliente MQTT
client.loop_start()

""" for i in range(len(datos)):
        pot.append(float(datos[i])) """

# Bucle infinito que se ejecutará cada 2 segundos para mostrar los mensajes
while True:
    tiempo_transcurrido = time.time() - tiempo_inicial

    datos.append(mensaje_3)
    pot = ([float(i) for i in datos if i != 0])
    print('lista potencia:', pot)

    promPot = (sum(pot)/len(datos))/1000
    costoEnergia = ((sum(pot)/1000) * 0.016667) * costo
    print('costo energia:',costoEnergia)

    for i in range(len(pot)):
        if(pot[i] == 0):
             # Crea el mensaje a enviar
            mensaje = ''

            # Envía el mensaje
            server.sendmail(remitente, destinatario, 'No hay consumo de potencia')

            """ # Cierra la conexión con el servidor SMTP
            server.quit() """
       

    print('promedio potencia:', promPot)
    # Abre el archivo CSV en modo de añadir para agregar los nuevos datos
    with open('datos.csv', 'a', newline='') as archivo_csv:
        writer = csv.writer(archivo_csv)
        writer.writerow([promPot,costo, costoEnergia])

    # Crea un DataFrame con los datos del archivo CSV

        archivo_csv = pd.read_csv('datos.csv')
        archivo_csv= archivo_csv.dropna()
        
        total_rows=len(archivo_csv.axes[0])
        if total_rows > 10:
            Datos_numericos = ['promedioPotencia', 'costo']
            archivo_csv[Datos_numericos + ['costoEnergia']].describe()

            # Separate features and labels
            X, y = archivo_csv[['promedioPotencia','costo']].values, archivo_csv['costoEnergia'].values

            # Split data 70%-30% into training set and test set
            X_train, X_test, y_train, y_test = train_test_split(X, y, test_size=0.30, random_state=0)

            # Fit a linear regression model on the training set
            model = LinearRegression().fit(X_train, y_train)
            if tiempo_transcurrido >= 60:

                with open('datos.csv', 'a', newline='') as archivo_csv:
                    writer = csv.writer(archivo_csv)
                    writer.writerow([promPot,costo, costoEnergia])

                pot.clear()
                datos.clear()
                
                predictions = model.predict(X_test)
                np.set_printoptions(suppress=True)

                write_api = client1.write_api(write_options=SYNCHRONOUS)

                # Crear punto de datos
                point = Point("Analitica").tag("ubicacion", "cuarto").field("prediccion", predictions[0]).field("promedioPotencia", promPot)

                # Escribir los datos en la base de datos
                write_api.write(bucket=bucket, org=org, record=point)
                with open('datos.csv', 'w', newline='') as archivo_csv:
                    writer = csv.writer(archivo_csv)
                    writer.writerow(['promedioPotencia', 'costo', 'costoEnergia'])
                tiempo_inicial = time.time()

    time.sleep(2)    
