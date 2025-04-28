from flask import Flask, request, jsonify
import os

app = Flask(__name__)

UPLOAD_FOLDER = './uploaded_files'
if not os.path.exists(UPLOAD_FOLDER):
    os.makedirs(UPLOAD_FOLDER)

@app.route('/', methods=['GET'])
def test():
    return "Server test", 200

@app.route('/agent/recheck', methods=['POST'])
def aoi_api():
    try:
        cmd = request.form.get('cmd')

        if 'file1' not in request.files:
            return jsonify({'status': 'failure', 'error': 'No file detected'}), 402
        file1 = request.files['file1']
        file1_path = os.path.join(UPLOAD_FOLDER, 'file.csv')
        file1.save(file1_path)

        return jsonify({
            'status':'success',
            'cmd': cmd,
            'file_path': file1_path,
        })
    except Exception as e :
        return jsonify({'status': 'failure', 'error': str(e)}), 500

if __name__ == '__main__':
    app.run(host='0.0.0.0', port=8080, debug=True)