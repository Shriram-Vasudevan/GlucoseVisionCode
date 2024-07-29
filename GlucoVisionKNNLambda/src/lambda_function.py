import json
import numpy as np
import sklearn
import joblib

def lambda_handler(event, context):
    input_data = event["input"]

    input_data = np.array(input_data).reshape(1, -1)

    model = joblib.load("model/knn_model.pkl")
    prediction = model.predict(input_data)

    prediction_list = prediction.tolist() 
   
    
    return {
        'statusCode': 200,
        'body': json.dumps(prediction_list[0])
    }



    
