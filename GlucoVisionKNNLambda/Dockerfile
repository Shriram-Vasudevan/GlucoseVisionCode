FROM amazon/aws-lambda-python:3.11

RUN yum update -y && \
    yum install -y mesa-libGL && \
    yum clean all
    
RUN /var/lang/bin/python3.11 -m pip install --upgrade pip

COPY requirements.txt .
RUN pip install -r requirements.txt

COPY src/ .

CMD ["lambda_function.lambda_handler"]

