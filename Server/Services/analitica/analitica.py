import paho.mqtt.client as mqtt
import time
import csv
import pandas as pd
from sklearn.linear_model import LinearRegression
import numpy as np
from sklearn.model_selection import train_test_split
from sklearn.metrics import mean_squared_error, r2_score
from influxdb_client.client.write_api import SYNCHRONOUS
from influxdb_client import InfluxDBClient, Point, WritePrecision

token = "token-secreto"
org = "org"
bucket = "RabbitBucket"
client1 = InfluxDBClient(url="http://4.204.226.150:8086", token=token)
datos = []
pot = []



# Define las variables que se usarán para guardar los mensajes
mensaje_1 = '' #Valor voltaje
mensaje_2 = '' #Valor Corriente
mensaje_3 = '' #Valor potencia
costo = 800
promPot = 0
costoEnergia= 0





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

"""     for element, i in datos:
        if datos[i] != '':
            pot.append(float(element))  """

# Bucle infinito que se ejecutará cada 5 segundos para mostrar los mensajes
while True:
    datos.append(mensaje_3)
    print(datos)

    for i in dir(datos):
        if datos[i] != '':
            pot.append(float(datos[i]))

    promedioPot = sum(pot)/len(datos)

    print('Promedio potencia:', promedioPot)
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
            predictions = model.predict(X_test)
            np.set_printoptions(suppress=True)
            mse = mean_squared_error(y_test, predictions)
            rmse = np.sqrt(mse)
            r2 = r2_score(y_test, predictions)
        

            write_api = client1.write_api(write_options=SYNCHRONOUS)

            # Crear punto de datos
            point = Point("Analitica").tag("ubicacion", "cuarto").field("prediccion", predictions[0]).field("mse", mse).field("rmse", rmse).field("r2", r2)

            # Escribir los datos en la base de datos
            write_api.write(bucket=bucket, org=org, record=point)

    time.sleep(5)    
