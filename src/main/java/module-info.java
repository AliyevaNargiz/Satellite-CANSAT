module com.example.station {
    requires javafx.controls;
    requires javafx.fxml;


    opens com.example.station to javafx.fxml;
    exports com.example.station;
}