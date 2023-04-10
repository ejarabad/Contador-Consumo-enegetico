FROM python:3.9

WORKDIR /app

RUN apt-get update
#RUN apt -y install python3-aiohttp
RUN pip install influxdb-client==1.36
RUN pip install paho-mqtt
RUN pip install pandas
RUN pip install numpy
RUN pip install scikit-learn

COPY Services/analitica/analitica.py /app/analitica.py
CMD ["python", "/app/analitica.py"]

