#include<bits/stdc++.h>
using namespace std;

// Structure to store vital information
struct Vitals{
	string vital_id;
	double value;
	string timestamp;
	Vitals(string _vital_id, double _value, string _timestamp){
		vital_id = _vital_id;
		value = _value;
		timestamp = _timestamp;
	}
};

// Class representing user data and vitals management operations
class Users{
public:
	string user_name;
	int age;
	string gender;
	vector<Vitals> vitals;
	Users() : user_name(""), age(0), gender("") {}
	Users(string _user_name, int _age, string _gender){
		user_name = _user_name;
		age = _age;
		gender = _gender;
	}
	void addVital(string vital_id, double value, string timestamp){
		vitals.push_back(Vitals(vital_id, value, timestamp));
	}
};

// Class containing common methods used in vital management operations
class Common_Methods{
public:
	bool check_timestamp(string timestamp){
		regex timestampRegex("^\\d{4}-\\d{2}-\\d{2}T\\d{2}:\\d{2}:\\d{2}Z$");
		bool is_valid_timestamp = regex_match(timestamp, timestampRegex);
		return is_valid_timestamp;
	}
	bool check_date(string date){
		regex dateRegex(R"(\d{4}-\d{2}-\d{2})");
		bool is_valid_date = regex_match(date, dateRegex);
		return is_valid_date;
	}
	string to_lower(string inpstring){
		transform(inpstring.begin(), inpstring.end(), inpstring.begin(), ::tolower);
		return inpstring;
	}
	bool check_gender(string &gender){
		gender = to_lower(gender);
		if(gender!="male" && gender!="female") return false;
		return true;
	}
	bool check_vital(string &vital_id){
		vital_id = to_lower(vital_id); 
		if(vital_id!="heart_rate" && vital_id!="temperature") return false;
		return true;
	}
};

// Class managing user data and vital management operations
class Vitals_management{
private:
	unordered_map<string, Users> user_dictionary;
	string status = "", message = "";
	Common_Methods cm;
	
	bool check_user(string user_name){
		if(user_dictionary.find(user_name)==user_dictionary.end()) return false;
		return true;
	}

public:
	// Method to create a new user
	string create_user(string user_name, int age, string gender){
		if(cm.check_gender(gender)==false){
			status = "Failed";
			message = "Gender must be male or female";
		}
		else if(check_user(user_name)==true){
			status = "Failed";
			message = "User already exists!";
		}
		else{
			user_dictionary[user_name] = Users(user_name, age, gender);
			status = "Success";
			message = "User " + user_name + " created!";
		}
		return "{\"status\": \"" + status + "\", \"message\": \"" + message + "\"}";
	}

	// Method to insert vital data for a user
	string insert_vital(string user_name, string vital_id, double value, string timestamp){
		if(cm.check_timestamp(timestamp)==false){
			status = "Failed";
			message = "Incorrect timestamp " + timestamp + " for user " + user_name;
		}
		else if(cm.check_vital(vital_id)==false){
			status = "Failed";
			message = "Incorrect vitalid " + vital_id + " for user " + user_name;
		}
		else if(check_user(user_name)==false){
			status = "Failed";
			message = "No user name " + user_name + " found!";
		}
		else{
			user_dictionary[user_name].addVital(vital_id, value, timestamp);
			status = "Success";
			message = "Vital inserted for " + user_name;
		}
		return "{\"status\": \"" + status + "\", \"message\": \"" + message + "\"}";
	}

	// Method to retrieve vital data for a user within a specific date range
	string get_vitals(string user_name, const vector<string> &dates){
		string start_date = dates[0];
		string end_date = dates[1];
		if(cm.check_date(start_date)==false || cm.check_date(end_date)==false || start_date>end_date){
			status = "Failed";
			message = "Invalid start_date " + start_date + " or end_date " + end_date;
		}
		else if(check_user(user_name)==false){
			status = "Failed";
			message = "No user name " + user_name + " found!";
		}
		else{
			vector<Vitals> output_message;
			for(auto &vital: user_dictionary[user_name].vitals){
				if(vital.timestamp>=start_date || vital.timestamp<=end_date){
					output_message.push_back(vital);
				}
			}
			status = "Success"; message="";
			if(output_message.size()==0){
				message = "No vitals found for user_name " + user_name + " between " + start_date + " and " + end_date;
			}
			else{
				message += "[";
				for(auto &vital : output_message){
					stringstream stream;
					stream << fixed << setprecision(2) << vital.value;
					message += "{\"vitalId\": \"" + vital.vital_id + "\", \"value\": " + stream.str() +
                               ", \"timestamp\": \"" + vital.timestamp + "\"}, ";
				}
				message.pop_back();
				message.pop_back();
			}
		}
		return "{\"status\": \"" + status + "\", \"message\": \"" + message + "\"}";
	}

	// Method to delete vital data for a specific user, vital type, and timestamp
	string delete_vitals(string user_name, string vital_id, string timestamp){
		vital_id = cm.to_lower(vital_id);
		if(cm.check_timestamp(timestamp)==false){
			status = "Failed";
			message = "Incorrect timestamp " + timestamp + " for user " + user_name;
		}
		else if(cm.check_vital(vital_id)==false){
			status = "Failed";
			message = "Incorrect vitalid " + vital_id + " for user " + user_name;
		}
		else if(check_user(user_name)==false){
			status = "Failed";
			message = "No user name " + user_name + " found!";
		}
		else{
			vector<Vitals> new_vitals;
			auto &user_vitals = user_dictionary[user_name].vitals; 
			for(auto &vital: user_vitals){
				if(vital_id != vital.vital_id && timestamp != vital.timestamp){
					new_vitals.push_back(vital);
				}
			}
			user_dictionary[user_name].vitals = new_vitals;
			status = "Success";
			message = "Vital deleted for " + user_name;
		}
		return "{\"status\": \"" + status + "\", \"message\": \"" + message + "\"}";
	}

	// Method to calculate aggregate values for a specific user, vital types, and date range
	string aggregate(string user_name, const vector<string> &vital_ids, const vector<string> &timestamp){
		string vital_id_heartrate = vital_ids[0];
		string vital_id_temperature = vital_ids[1];
		string start_date = timestamp[0];
		string end_date = timestamp[1];
		if(check_user(user_name)==false){
			status = "Failed";
			message = "No user name " + user_name + " found!";
		}
		else if(cm.check_vital(vital_id_heartrate)==false || cm.check_vital(vital_id_temperature)==false){
			status = "Failed";
			message = "Incorrect vitalid " + vital_id_heartrate + " or " + vital_id_temperature + " for user " + user_name;
		}
		else if(cm.check_timestamp(start_date)==false || cm.check_timestamp(end_date)==false){
			status = "Failed";
			message = "Incorrect timestamp " + start_date + " or " + end_date + " for user " + user_name;
		}
		else{
			auto &user_vitals = user_dictionary[user_name].vitals;
			double sum_temperature = 0, sum_heartrate = 0;
			double count_temperature = 0, count_heartrate = 0;
			for(auto &vital: user_vitals){
				if(vital.timestamp >=start_date && vital.timestamp<=end_date){
					if(vital.vital_id == vital_id_heartrate){
						sum_heartrate += vital.value;
						count_heartrate += 1;
					}
					else if(vital.vital_id == vital_id_temperature){
						sum_temperature += vital.value;
						count_temperature += 1;
					}
				}
			}
			double aggregate_heartrate = 0.0;
			double aggregate_temperature = 0.0;
			if (count_heartrate > 0) {
				aggregate_heartrate = sum_heartrate / count_heartrate;
			}
			if (count_temperature > 0) {
				aggregate_temperature = sum_temperature / count_temperature;
			}
			stringstream stream1, stream2;
			stream1 << fixed << setprecision(2) << aggregate_heartrate;
			stream2 << fixed << setprecision(2) << aggregate_temperature;
			status = "Success";
			message = "{\"message\": \"Aggregate fetched successfully.\", \"data\": {\"username\": \"" + user_name + "\", \"aggregates\": {\"HeartRate\": " + stream1.str() + ", \"Temperature\": " + stream2.str() + "}, \"start_timestamp\": \"" + start_date + "\", \"end_timestamp\": \"" + end_date + "\"}}";
		}
		return "{\"status\": \"" + status + "\", \"message\": \"" + message + "\"}";
	}

	// Method to calculate population insights for a specific user, vital type, and date range
	string population_insight(string user_name, string vital_id, const vector<string>&timestamp){
		string start_timestamp = timestamp[0];
		string end_timestamp = timestamp[1];
		if(check_user(user_name)==false){
			status = "Failed";
			message = "No user name " + user_name + " found!";
		}
		else if(cm.check_vital(vital_id)==false){
			status = "Failed";
			message = "Incorrect vitalid " + vital_id;
		}
		else if(cm.check_timestamp(start_timestamp)==false || cm.check_timestamp(end_timestamp)==false){
			status = "Failed";
			message = "Incorrect timestamp " + start_timestamp + " or " + end_timestamp + " for user " + user_name;
		}
		else{
			vector<double>below_user;
			for(auto &user: user_dictionary){
				string cur_user = user.first;
				auto &cur_user_vitals = user_dictionary[cur_user].vitals;
				double sum_vital = 0;
				int count_vital = 0;
				for(auto &vital: cur_user_vitals){
					if(vital.timestamp>=start_timestamp && vital.timestamp<=end_timestamp && vital.vital_id==vital_id){
						sum_vital += vital.value;
						count_vital += 1;
					}
				}
				double aggregate_vital = 0;
				if (count_vital > 0) {
					aggregate_vital = sum_vital / count_vital;
					below_user.push_back(aggregate_vital);
				}
			}
			auto &user_name_vitals = user_dictionary[user_name].vitals;
			double sum_vital = 0;
			int count_vital = 0;
			for(auto &vital: user_name_vitals){
				if(vital.timestamp>=start_timestamp && vital.timestamp<=end_timestamp && vital.vital_id==vital_id){
					sum_vital += vital.value;
					count_vital += 1;
				}
			}
			double aggregate_vital = 0;
			if (count_vital > 0) {
				aggregate_vital = sum_vital / count_vital;
			}
			int count_less_than_user = 0;
			for(int i=0; i<below_user.size(); i++){
				if(below_user[i]<=aggregate_vital) count_less_than_user++;
			}
			double percentile = 0;
			if(count_less_than_user>0 && below_user.size()>0) percentile = double(count_less_than_user)/(below_user.size());
			percentile *= 100.00;
			stringstream stream2;
			stream2 << fixed << setprecision(2) << percentile;
			status = "Success";
			message = "{\"message\": \"Population insight fetched successfully.\", \"data\": {\"username\": \"" + user_name + "\", \"vital_id\": \"" + vital_id + "\", \"start_timestamp\": \"" + start_timestamp + "\", \"end_timestamp\": \"" + end_timestamp + "\", \"insight\": \"Your " + vital_id + " is in the " + stream2.str() + " percentile.\"}}";
		}
		return "{\"status\": \"" + status + "\", \"message\": \"" + message + "\"}";
	}

};

int main(){
	// Creating an instance of Vitals_management class
	Vitals_management vm;

	// Sample input JSON data containing commands and parameters
	vector<unordered_map<string, string>> inputJSON = {
		{{"command", "create_user"}, {"username", "Alice"}, {"age", "25"}, {"gender", "Female"}},
		{{"command", "create_user"}, {"username", "Bob"}, {"age", "28"}, {"gender", "Male"}},
		{{"command", "create_user"}, {"username", "Tom"}, {"age", "35"}, {"gender", "Male"}},
		{{"command", "insert_vital"}, {"username", "Alice"}, {"vital_id", "heart_rate"}, {"value", "75"}, {"timestamp", "2023-10-01T12:30:00Z"}},
		{{"command", "insert_vital"}, {"username", "Alice"}, {"vital_id", "Temperature"}, {"value", "98.6"}, {"timestamp", "2023-10-02T12:30:00Z"}},
		{{"command", "insert_vital"}, {"username", "Alice"}, {"vital_id", "heart_rate"}, {"value", "80"}, {"timestamp", "2023-10-03T14:30:00Z"}},
		{{"command", "insert_vital"}, {"username", "Bob"}, {"vital_id", "heart_rate"}, {"value", "82"}, {"timestamp", "2023-10-01T13:30:00Z"}},
		{{"command", "insert_vital"}, {"username", "Tom"}, {"vital_id", "heart_rate"}, {"value", "89"}, {"timestamp", "2023-10-04T15:30:00Z"}},
		{{"command", "aggregate"}, {"username", "Alice"}, {"vital_ids", "heart_rate,Temperature"}, {"start_timestamp", "2023-10-01T00:00:00Z"}, {"end_timestamp", "2023-10-31T23:59:59Z"}},
		{{"command", "population_insight"}, {"username", "Alice"}, {"vital_id", "heart_rate"}, {"start_timestamp", "2023-10-01T00:00:00Z"}, {"end_timestamp", "2023-10-31T23:59:59Z"}}
	};

	// Loop through the input JSON data and execute corresponding commands
	for (const auto& commandObject : inputJSON) {
		string commandType = commandObject.at("command");

		if (commandType == "create_user") {
			cout << vm.create_user(commandObject.at("username"), stoi(commandObject.at("age")), commandObject.at("gender"))<< endl<< endl;
		} else if (commandType == "insert_vital") {
			cout << vm.insert_vital(commandObject.at("username"), commandObject.at("vital_id"), stod(commandObject.at("value")), commandObject.at("timestamp")) << endl<< endl;
		} else if (commandType == "get_vitals") {
			cout << vm.get_vitals(commandObject.at("username"), {commandObject.at("start_timestamp"), commandObject.at("end_timestamp")}) << endl<< endl;
		} else if (commandType == "delete_vital") {
			cout<<commandObject.at("vital_id")<<endl;
			cout << vm.delete_vitals(commandObject.at("username"), commandObject.at("vital_id"), commandObject.at("timestamp")) << endl<< endl;
		} else if (commandType == "aggregate") {
			size_t commaPos = commandObject.at("vital_ids").find(',');
			string heartRate = "";
			string temperature = ""; 
			if (commaPos != std::string::npos) {
				heartRate = commandObject.at("vital_ids").substr(0, commaPos);
				temperature = commandObject.at("vital_ids").substr(commaPos + 1);
			}
			cout << vm.aggregate(commandObject.at("username"), {heartRate, temperature}, {commandObject.at("start_timestamp"), commandObject.at("end_timestamp")}) << endl<< endl;
		} else if (commandType == "population_insight") {
			cout << vm.population_insight(commandObject.at("username"), commandObject.at("vital_id"), {commandObject.at("start_timestamp"), commandObject.at("end_timestamp")}) << endl<< endl;
		} else {
			cout << "Invalid command type: " << commandType << endl<< endl;
		}
	}

	return 0;
}
