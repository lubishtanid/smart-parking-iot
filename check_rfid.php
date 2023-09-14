<?php
//db conf
$servername = "localhost";
$username = "root";
$password = "";
$database = "iot_members";

//creating a connection to the db
$conn = new mysqli($servername, $username, $password, $database);

//check connection
if ($conn->connect_error){
    die("Connection failed: " . $conn->connect_error);
}

//getting UID from the RFID scanner via a POST request
$rfid_uid = $_POST['rfid_uid'];

//query to check if uid exists
$sql = "SELECT * from parking_members WHERE card_uid= '$rfid_uid'";
$result = $conn->query($sql);

if ($result->num_rows>0){
    //row with this UID exists in the table
    echo "true";
}
else{
    //UID does not exist in the database
    echo "false";
}

$conn->close();
?>