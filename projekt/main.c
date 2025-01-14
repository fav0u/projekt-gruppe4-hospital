#include <stdio.h>
#include <time.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <unistd.h>

#define MAX_REG 100
#define MAX_SEAT 25
#define MAX_PAT 100
#define MAX_ER 10
#define MAX_NEIGH 100

typedef struct{
    char svNr[11];
    char vorname[20];
    char nachname[20];
    char gebdat[20];
    char telefon[20];
    char email[40];
    char arrival_method[20];
    time_t date_of_arrival;
    char status[20];
}Patient;

typedef struct{
    //char id[5];
    int row;
    int spot;
    char status[20];
}Seat;

typedef struct {
  int room;
  char status[20];
}ER;

typedef struct {
    char pID[11];
    int seat_row;
    int seat_spot;
    time_t start_time;
    time_t end_time;
}Register;

typedef struct{
    char ID[11];
    char n1[11];
    char n2[11];
    char n3[11];
    char n4[11];

}Neighbour;

void reset_seats(Seat seats[MAX_SEAT],Patient patients[MAX_PAT]){
    for (int i = 0; i < 5; ++i) {
        for (int j = 0; j < 5; ++j) {
            int index = i * 5 + j;
            seats[index].row = i+1;
            seats[index].spot = j+1;
        }
    }
    FILE *fpointer_seat, *fpointer_patients, *fpointer_other;
    fpointer_seat= fopen("seats.txt","w");
    for (int i = 0; i < MAX_SEAT; ++i) {
        fprintf(fpointer_seat,"%d,%d,Free;\n",
                seats[i].row,seats[i].spot);
    }
    fclose(fpointer_seat);

    int k=0;
    while (strcmp(patients[k].svNr,"")!=0){
        if (strcmp(patients[k].status,"Seating")==0){
            strcpy(patients[k].status,"-");
        }
        k++;
    }
    fpointer_patients= fopen("patients/patients.txt","w");
    k=0;
    while (strcmp(patients[k].svNr,"")!=0){
        fprintf(fpointer_patients,"%s,%s,%s,%s,%s,%s,%s,%lld,%s;\n",
                patients[k].svNr,patients[k].vorname,patients[k].nachname,patients[k].gebdat,
                patients[k].telefon,patients[k].email, patients[k].arrival_method,
                patients[k].date_of_arrival, patients[k].status);
        k++;
    }
    fclose(fpointer_patients);

    fpointer_other= fopen("patients/patients_other","w");

    for (int i = 0; i < MAX_PAT; ++i) {
        if(strcmp(patients[i].svNr,"")!=0 && strcmp(patients[i].status,"Ambulance")!=0){
            fprintf(fpointer_patients,"%s,%s,%s,%s,%s,%s,%s,%lld,%s;\n",
                    patients[i].svNr,patients[i].vorname,patients[i].nachname,patients[i].gebdat,
                    patients[i].telefon,patients[i].email, patients[i].arrival_method,
                    patients[i].date_of_arrival, patients[i].status);
        }
    }
    fclose(fpointer_other);
    printf("All seats are free!\n");
}

void reset_er(ER er[MAX_ER]){
    for (int i = 0; i < MAX_ER; ++i) {
        er[i].room=i+1;
    }
    FILE *fpointer_er = fopen("er.txt","w");
    for (int i = 0; i < MAX_ER; ++i) {
        fprintf(fpointer_er,"%d,Free;\n",
                er[i].room);
    }
    fclose(fpointer_er);
}

int load_register(Register reg[MAX_REG], Seat seats[MAX_SEAT], int record){

    for (int i = 0; i < MAX_REG; ++i) {
        strcpy(reg[i].pID,"");
    }

    FILE *fpointer_register;
    fpointer_register= fopen("register.txt","r");

    int read;
    record=0;
    int flag=0;


    do {
        char row_buffer, spot_buffer, start_buffer[20], end_buffer[20];

            read= fscanf(fpointer_register, "%29[^,],%c,%c,%29[^,],%29[^;]%*[\n;]",
                         reg[record].pID, &row_buffer, &spot_buffer, start_buffer, end_buffer);

        reg[record].seat_row = row_buffer - '0';
        reg[record].seat_spot = spot_buffer - '0';
        reg[record].start_time = strtoll(start_buffer, NULL, 10);
        reg[record].end_time = strtoll(end_buffer, NULL, 10);

        if (read==5) record++;
        else {
            printf("Error loading register.txt! read:%i\n", read);
            flag=1;
            break;
        }
        if (ferror(fpointer_register)) {
            printf("Error opening file!");
            return 1;
        }

    } while (!feof(fpointer_register));

    if (flag==0)
        printf("register.txt opened successfully!\n\n");
    fclose(fpointer_register);

    return 0;
}

void load_patient_data(Patient patients[MAX_PAT], int patient_record){

    for (int i = 0; i < MAX_PAT; ++i) {
        strcpy(patients[i].svNr,"");
    }

    FILE *fpointer_ambulance= fopen("patients/patients_ambulance.txt", "r");
    int read;
    char date_of_arrival_str[20];
    patient_record=0;

    do {
        read= fscanf(fpointer_ambulance, "%50[^,],%50[^,],%50[^,],%20[^,],%50[^,],%50[^,],%50[^,],%50[^,],%50[^;\n]%*[\n;]",
                     patients[patient_record].svNr, patients[patient_record].vorname, patients[patient_record].nachname,
                     patients[patient_record].gebdat, patients[patient_record].telefon, patients[patient_record].email,
                     patients[patient_record].arrival_method, date_of_arrival_str, patients[patient_record].status);

        if(read==9) {
            patients[patient_record].date_of_arrival = strtoll(date_of_arrival_str, NULL, 10);
            patient_record++;
        }
        else printf("Formatting error! (patients_ambulance.txt) read=%d\n",read);

    } while (!feof(fpointer_ambulance));
    fclose(fpointer_ambulance);


    FILE *fpointer_other= fopen("patients/patients_other.txt", "r");

    do {
        read= fscanf(fpointer_other, "%50[^,],%50[^,],%50[^,],%20[^,],%50[^,],%50[^,],%50[^,],%50[^,],%50[^;\n]%*[\n;]",
                     patients[patient_record].svNr, patients[patient_record].vorname, patients[patient_record].nachname,
                     patients[patient_record].gebdat, patients[patient_record].telefon, patients[patient_record].email,
                     patients[patient_record].arrival_method, date_of_arrival_str, patients[patient_record].status);

        if(read==9) {
            patients[patient_record].date_of_arrival = strtoll(date_of_arrival_str, NULL, 10);
            patient_record++;
        }
        else printf("Formatting error! (patients_other.txt) read=%d\n",read);

    } while (!feof(fpointer_other));
    fclose(fpointer_other);

    printf("Patient data loaded! (record: %i)\n",patient_record);
}

void load_seat_data(Seat seats[MAX_SEAT]){
    FILE *fpointer_seat;
    fpointer_seat= fopen("seats.txt","r");

    if (fpointer_seat == NULL) {
        printf("Failed to open the file.\n");
        return;
    }

    int read;
    int record=0;

    do{
        char row_buffer=0;
        char spot_buffer=0;

        read= fscanf(fpointer_seat, "%c,%c,%29[^;\n]%*[\n;]",
                     &row_buffer, &spot_buffer, seats[record].status);

        //printf("row_buffer=%c, spot_buffer=%c, status=%s\n", row_buffer, spot_buffer, seats[record].status);


        int row_value =row_buffer - '0';
        int spot_value =spot_buffer - '0';
        seats[record].row = row_value;
        seats[record].spot = spot_value;

        //printf("row_value=%d, spot_value=%d\n", row_value, spot_value);

        if (read==3) record++;
        else{
            printf("Formatting error!\n");
            printf("read = %d\n", read);
            printf("R%dS%d - %s\n",row_value,spot_value,seats[record].status);
            break;
        }

    } while (!feof(fpointer_seat));

    fclose(fpointer_seat);

    /*for (int i = 0; i < MAX_SEAT; ++i) {
        printf("%d,%d,%s;\n",seats[i].row,seats[i].spot,seats[i].status);
    }*/


    if(record==25)
        printf("\nSeat data loaded!\n\n");
    else
        printf("Seat data could not be loaded!");
}

void load_er_data(ER er[MAX_ER]){

    FILE *fpointer_er = fopen("er.txt", "r");

    if (fpointer_er == NULL) {
        printf("Failed to open the file.\n");
        return;
    }

    int read;
    int record=0;

    do{
        char room_buffer[3];

        read= fscanf(fpointer_er, "%3[^,],%29[^;\n]%*[\n;]",
                     room_buffer, er[record].status);


        int room_value = strtol(room_buffer, NULL, 10);
        er[record].room = room_value;

        //printf("row_value=%d, spot_value=%d\n", row_value, spot_value);

        if (read==2) record++;
        else{
            printf("Formatting error!\n");
            printf("read = %d\n", read);
            printf("R%d - %s\n",room_value,er[record].status);
            break;
        }

    } while (!feof(fpointer_er));

    fclose(fpointer_er);

    if(record==10)
        printf("\nER data loaded! (record: %i)\n\n",record);
    else
        printf("ER data could not be loaded! \nrecord");
}


//add function to check if Social-Security number exist
void assign_patient_to_seat(Seat seats[MAX_SEAT], Register reg[MAX_REG], Patient patients[MAX_PAT], int record){

    //load_register(reg, seats, record);

    int occupied_counter=0;
    int seat_flag=0;
    int fail_counter=0;
    char input;

    for (int i = 0; i < MAX_SEAT; ++i) {
        if(strcmp(seats[i].status,"Free")==0) {                  //check for seat that isn't occupied
            printf("[1] Automatic assignment \n[2] Assign seat manually\n");
            scanf(" %c", &input);

            char temp[11];
            int input_invalid = 1;

            while (input_invalid == 1){
                if (input == '1') {
                    for (int j = 0; j < MAX_PAT; ++j) {
                        if (strcmp(patients[j].status, "Seating") != 0 &&
                            strcmp(patients[j].arrival_method, "Other") == 0) {
                            strcpy(temp, patients[j].svNr);
                            break;
                        }
                    }
                    while (seat_flag == 0) {
                        strcpy(reg[record].pID, temp);
                        fail_counter = 0;

                        for (int j = 0; j < MAX_PAT; ++j) {
                            if (strcmp(reg[record].pID, patients[j].svNr) == 0) {

                                if (strcmp(patients[j].status, "Seating") == 0) {
                                    printf("Patient is already seated!\n");
                                    seat_flag = 0;
                                } else if (strcmp(patients[j].arrival_method, "Ambulance") == 0) {
                                    printf("Patient is not eligible for a seat!\n");
                                    seat_flag = 0;
                                } else if (strcmp(patients[j].svNr, "") == 0){
                                    printf("No eligable Patients found!");
                                    return;
                                } else {
                                    seat_flag = 1;
                                    strcpy(patients[j].status, "Seating");
                                }
                            } else fail_counter++;
                        }
                        if (fail_counter >= MAX_PAT) {
                            printf("There is no patient with this social security number!\n");
                            seat_flag = 0;
                        }
                    }
                    input_invalid = 0;
                } else if (input == '2') {
                    while (seat_flag == 0) {
                        printf("Social security number of patient: ");
                        scanf("%11s", reg[record].pID);
                        fail_counter = 0;

                        for (int j = 0; j < MAX_PAT; ++j) {
                            if (strcmp(reg[record].pID, patients[j].svNr) == 0) {

                                if (strcmp(patients[j].status, "Seating") == 0) {
                                    printf("Patient is already seated!\n");
                                    seat_flag = 0;
                                } else if (strcmp(patients[j].arrival_method, "Ambulance") == 0) {
                                    printf("Patient is not eligible for a seat!\n");
                                    seat_flag = 0;
                                } else {
                                    seat_flag = 1;
                                    strcpy(patients[j].status, "Seating");
                                }
                            } else fail_counter++;
                        }
                        if (fail_counter >= MAX_PAT) {
                            printf("There is no patient with this social security number!\n");
                            seat_flag = 0;
                        }
                    }
                    input_invalid = 0;
                } else{
                    printf("Invalid Input!");
                    input_invalid = 1;
                }
        }


            strcpy(seats[i].status,reg[record].pID);

            reg[record].start_time = time(NULL);
            reg[record].end_time = time(NULL);

            FILE *fpointer_reg;
            //char *string_start= ctime(&reg[record].start_time);
            //char *string_end= ctime(&reg[record].end_time);

            fpointer_reg= fopen("register.txt", "a");

            if (fpointer_reg == NULL){
                printf("Datei nicht gefunden");
            }

            fprintf(fpointer_reg, "%s,%d,%d,%lld,%lld;\n",
                    reg[record].pID, seats[i].row, seats[i].spot, reg[record].start_time, reg[record].end_time);

            printf("Patient: %s has been asssigned seat R%dS%d at %s\n",reg[record].pID,seats[i].row,seats[i].spot,
                   ctime(&reg[record].start_time));

            fclose(fpointer_reg);
            break;
        } else occupied_counter++;
    }
    if (occupied_counter>=MAX_SEAT)
        printf("All seats are currently occupied!");

    //Overwriting existing seats.txt file with new data
    FILE *fpointer_seats;
    fpointer_seats= fopen("seats.txt","w");

    for (int i = 0; i < MAX_SEAT; ++i) {
        fprintf(fpointer_seats,"%d,%d,%s;\n",seats[i].row,seats[i].spot,seats[i].status);
    }

    fclose(fpointer_seats);

    //Overwriting existing patient.txt file with new data
    FILE *fpointer_patient= fopen("patients/patients.txt","w");

    int patient_record=0;

    while (strcmp(patients[patient_record].svNr,"")!=0){
        fprintf(fpointer_patient,"%s,%s,%s,%s,%s,%s,%s,%lld,%s;\n",
                patients[patient_record].svNr,patients[patient_record].vorname,patients[patient_record].nachname,patients[patient_record].gebdat,
                patients[patient_record].telefon,patients[patient_record].email, patients[patient_record].arrival_method,
                patients[patient_record].date_of_arrival, patients[patient_record].status);
        patient_record++;
    }
    fclose(fpointer_patient);

    FILE *fpointer_other= fopen("patients/patients_other.txt","w");

    int other_record=0;

    while (strcmp(patients[other_record].svNr,"")!=0){
        if(strcmp(patients[other_record].arrival_method,"Other")==0) {
            fprintf(fpointer_other, "%s,%s,%s,%s,%s,%s,%s,%lld,%s;\n",
                    patients[other_record].svNr, patients[other_record].vorname, patients[other_record].nachname,
                    patients[other_record].gebdat,patients[other_record].telefon, patients[other_record].email,
                    patients[other_record].arrival_method,patients[patient_record].date_of_arrival, patients[other_record].status);
        }
        other_record++;
    }
    fclose(fpointer_other);
}

//add function to check if Social-Security number exist
void send_patient_to_er(Seat seats[MAX_SEAT], Register reg[MAX_REG], Patient patients[MAX_PAT], ER er[MAX_ER]){

    int record =0;
    int seat_flag=0;
    int fail_counter=0;
    //load_register(reg, seats, record);

    char input[11], menu_input;
    long long start;

    printf("[1] Automatic assignment \n[2] Assign Patient manually\n");
    scanf(" %c", &menu_input);

    if (menu_input=='2'){
        while (seat_flag==0) {
            fail_counter=0;
            printf("Social security number of patient: ");
            scanf("%10s", input);

            for (int j = 0; j < MAX_PAT; ++j) {
                if (strcmp(patients[j].svNr, input) == 0) {
                    if (strcmp(patients[j].status, "in ER") == 0) {
                        printf("Patient is already in the ER!\n");
                        seat_flag=0;
                    } else {
                        seat_flag = 1;
                        strcpy(patients[j].status, "in ER");
                    }
                } else fail_counter++;
            }
            if (fail_counter>=MAX_PAT){
                printf("There is no patient with this social security number!\n");
                seat_flag=0;
            }
        }
    } else if (menu_input=='1'){
        fail_counter=0;
        for (int i = 0; i < MAX_PAT; ++i) {
            if (strcmp(patients[i].status,"Seating")==0 || (strcmp(patients[i].arrival_method,"Ambulance")==0 && strcmp(patients[i].status,"in ER")!=0)){
                strcpy(input,patients[i].svNr);
                break;
            }else fail_counter++;
        }
        if (fail_counter>=MAX_PAT){
            printf("No eligible Patients found!\n");
            return;
        } else{
            fail_counter=0;
            for (int j = 0; j < MAX_PAT; ++j) {
                if (strcmp(patients[j].svNr, input) == 0) {
                    if (strcmp(patients[j].status, "in ER") == 0) {
                        printf("Patient is already in the ER!\n");
                        return;
                    } else {
                        strcpy(patients[j].status, "in ER");
                    }
                } else fail_counter++;
            }
            if (fail_counter>=MAX_PAT){
                printf("There is no patient with this social security number!\n");
                return;
            }
        }

    }

fail_counter=0;
    for (int i = 0; i < MAX_ER; ++i) {
        if (strcmp(er[i].status,"Free")==0){
            strcpy(er[i].status,input);
            printf("Patient %s has been sent to ER %d\n",input,er[i].room);
            break;
        } else fail_counter++;
    }

    if (fail_counter>=MAX_ER){
        printf("All ER's are currently occupied!");
    }

    for (int i = 0; i < MAX_SEAT; ++i) {
        if (strcmp(seats[i].status,input)==0){
            strcpy(seats[i].status,"Free");
            break;
        }
    }

    for (int i = 0; i < MAX_REG; ++i) {
        if(strcmp(reg[i].pID,input)==0 && reg[i].start_time==reg[i].end_time){
            start=reg[i].start_time;
            reg[i].end_time= time(NULL);
            reg[i].start_time=start;
        }
    }

    //Overwrite existing register.txt file with new data
    FILE *fpointer_reg= fopen("register.txt","w");

    for (int i = 0; i < MAX_REG; ++i) {
        if(strcmp(reg[i].pID,"")!=0) {
            fprintf(fpointer_reg, "%s,%d,%d,%lld,%lld;\n",
                    reg[i].pID, reg[i].seat_row, reg[i].seat_spot, reg[i].start_time, reg[i].end_time);
        }
    }
    fclose(fpointer_reg);

    FILE *fpointer_seats;
    fpointer_seats= fopen("seats.txt","w");

    for (int i = 0; i < MAX_SEAT; ++i) {
        fprintf(fpointer_seats,"%d,%d,%s;\n",seats[i].row,seats[i].spot,seats[i].status);
    }

    fclose(fpointer_seats);

    //Overwriting existing patient.txt file with new data
    FILE *fpointer_patient= fopen("patients/patients.txt","w");

    int patient_record=0;

    while (strcmp(patients[patient_record].svNr,"")!=0){
        fprintf(fpointer_patient,"%s,%s,%s,%s,%s,%s,%s,%lld,%s;\n",
                patients[patient_record].svNr,patients[patient_record].vorname,patients[patient_record].nachname,patients[patient_record].gebdat,
                patients[patient_record].telefon,patients[patient_record].email, patients[patient_record].arrival_method,
                patients[patient_record].date_of_arrival, patients[patient_record].status);
        patient_record++;
    }
    fclose(fpointer_patient);

    FILE *fpointer_other= fopen("patients/patients_other.txt","w");

    int other_record=0;

    while (strcmp(patients[other_record].svNr,"")!=0){
        if(strcmp(patients[other_record].arrival_method,"Other")==0) {
            fprintf(fpointer_other, "%s,%s,%s,%s,%s,%s,%s,%lld,%s;\n",
                    patients[other_record].svNr, patients[other_record].vorname, patients[other_record].nachname,
                    patients[other_record].gebdat,patients[other_record].telefon, patients[other_record].email,
                    patients[other_record].arrival_method,patients[patient_record].date_of_arrival, patients[other_record].status);
        }
        other_record++;
    }
    fclose(fpointer_other);

    FILE *fpointer_ambulance= fopen("patients/patients_ambulance.txt","w");

    int ambulance_record=0;

    while (strcmp(patients[ambulance_record].svNr,"")!=0){
        if(strcmp(patients[ambulance_record].arrival_method,"Ambulance")==0) {
            fprintf(fpointer_ambulance, "%s,%s,%s,%s,%s,%s,%s,%lld,%s;\n",
                    patients[ambulance_record].svNr, patients[ambulance_record].vorname, patients[ambulance_record].nachname,
                    patients[ambulance_record].gebdat,patients[ambulance_record].telefon, patients[ambulance_record].email,
                    patients[ambulance_record].arrival_method,patients[ambulance_record].date_of_arrival, patients[ambulance_record].status);
        }
        ambulance_record++;
    }
    fclose(fpointer_ambulance);

    //Overwrite existing er.txt file
    FILE *fpointer_er= fopen("er.txt","w");

    for (int i = 0; i < MAX_ER; ++i) {
        fprintf(fpointer_er,"%d,%s;\n",
                er[i].room, er[i].status);
    }
    fclose(fpointer_er);
    printf("Success!");
}

void add_patient(Patient patients[MAX_PAT], int patient_record){
    FILE *fpointer_patient;
    FILE *fpointer_ambulance;
    FILE *fpointer_other;

    int flag =1;

    while (flag==1) {

        flag=0;
        char temp[11];

        printf("Social security number: ");
        scanf("%11s", temp);

        for (int i = 0; i < MAX_PAT; ++i) {
            if (strcmp(temp,patients[i].svNr)==0){
                printf("Social security number is already registered!\n");
                flag=1;
                break;
            } else{
                strcpy(patients[patient_record].svNr,temp);
                flag=0;
            }
        }
    }
    printf("\nFirst Name: ");
    scanf("%20s",patients[patient_record].vorname);

    printf("\nLast Name: ");
    scanf("%20s",patients[patient_record].nachname);

    printf("\nDate of Birth: ");
    scanf("%20s",patients[patient_record].gebdat);

    printf("\nTelephone number: ");
    scanf("%20s",patients[patient_record].telefon);

    printf("\nEmail address: ");
    scanf("%40s",patients[patient_record].email);

    flag = 1;
    while (flag==1) {
        printf("\nArrival Method (Ambulance/Other): ");
        scanf("%20s", patients[patient_record].arrival_method);
        if (strcmp(patients[patient_record].arrival_method,"Ambulance")!=0 && strcmp(patients[patient_record].arrival_method,"Other")!=0){
            printf("The mode of arrival must be 'Ambulance' or 'Other'");
            flag=1;
        } else flag=0;
    }
    strcpy(patients[patient_record].status,"-");

    fpointer_patient = fopen("patients/patients.txt","a");

    if(fpointer_patient==NULL){
        printf("Couldn't find file ---> creating new file\n");
    }

    patients[patient_record].date_of_arrival= time(NULL);

    fprintf(fpointer_patient,"%s,%s,%s,%s,%s,%s,%s,%lld,%s;\n",
            patients[patient_record].svNr,patients[patient_record].vorname,patients[patient_record].nachname,patients[patient_record].gebdat,
            patients[patient_record].telefon,patients[patient_record].email, patients[patient_record].arrival_method,
            patients[patient_record].date_of_arrival, patients[patient_record].status);

    fclose(fpointer_patient);

    if(strcmp(patients[patient_record].arrival_method,"Ambulance")==0){
        fpointer_ambulance= fopen("patients/patients_ambulance.txt","a");

        fprintf(fpointer_patient,"%s,%s,%s,%s,%s,%s,%s,%lld,%s;\n",
                patients[patient_record].svNr,patients[patient_record].vorname,patients[patient_record].nachname,patients[patient_record].gebdat,
                patients[patient_record].telefon,patients[patient_record].email, patients[patient_record].arrival_method,
                patients[patient_record].date_of_arrival, patients[patient_record].status);
        fclose(fpointer_ambulance);
    } else{
        fpointer_other= fopen("patients/patients_other.txt","a");

        fprintf(fpointer_patient,"%s,%s,%s,%s,%s,%s,%s,%lld,%s;\n",
                patients[patient_record].svNr,patients[patient_record].vorname,patients[patient_record].nachname,patients[patient_record].gebdat,
                patients[patient_record].telefon,patients[patient_record].email, patients[patient_record].arrival_method,
                patients[patient_record].date_of_arrival, patients[patient_record].status);
        fclose(fpointer_other);
    }

    printf("\nSocial security number: %s\n"
           "First Name: %s\n"
           "Last Name: %s\n"
           "Date of Birth: %s\n"
           "Telephone Number: %s\n"
           "Email address: %s\n"
           "Method of Arrival: %s\n"
           "Date of arrival: %s"
           "Status: %s\n",
           patients[patient_record].svNr,patients[patient_record].vorname,patients[patient_record].nachname,patients[patient_record].gebdat,
           patients[patient_record].telefon,patients[patient_record].email, patients[patient_record].arrival_method,
           ctime(&patients[patient_record].date_of_arrival) , patients[patient_record].status);

    printf("\nData successfully saved!");
}

void successful_treatment(Patient patients[MAX_PAT], ER er[MAX_ER]){

    char input[11];
    int flag=1;

    while(flag==1) {
        int fail_counter=0;

        printf("Enter Social Security Number: ");
        scanf("%s", input);

        for (int i = 0; i < MAX_ER; ++i) {
            if (strcmp(er[i].status, input) == 0) {
                printf("Match found!\n");
                strcpy(er[i].status, "Free");
                printf("%d - %s\n",er[i].room,er[i].status);
                flag=0;
                break;
            } else fail_counter++;
        }
        if (fail_counter++ >= MAX_ER) {
            printf("Patient isn't in the ER!\n");
            flag = 1;
        } else flag=0;
    }

    for (int i = 0; i < MAX_PAT; ++i) {
        if (strcmp(patients[i].svNr, input) == 0){
            strcpy(patients[i].status, "treated");
            FILE *fpointer_treated = fopen("patients/treated.txt","a");
            fprintf(fpointer_treated,"%s,%s,%s,%s,%s,%s,%s,%lld,%s;\n",
                    patients[i].svNr, patients[i].vorname, patients[i].nachname,
                    patients[i].gebdat,patients[i].telefon, patients[i].email,
                    patients[i].arrival_method,patients[i].date_of_arrival, patients[i].status);
            fclose(fpointer_treated);
            break;
        }
    }

    //Overwrite existing patients.txt file
    FILE *fpointer_patients = fopen("patients/patients.txt","w");

    for (int i = 0; i < MAX_PAT; ++i) {
        if ((strcmp(patients[i].status, "treated") != 0) && strcmp(patients[i].svNr, "")!=0){
            fprintf(fpointer_patients,"%s,%s,%s,%s,%s,%s,%s,%lld,%s;\n",
                    patients[i].svNr, patients[i].vorname, patients[i].nachname,
                    patients[i].gebdat,patients[i].telefon, patients[i].email,
                    patients[i].arrival_method,patients[i].date_of_arrival, patients[i].status);
        }
    }
    fclose(fpointer_patients);

    FILE *fpointer_other = fopen("patients/patients_other.txt","w");

    for (int i = 0; i < MAX_PAT; ++i) {
        if ((strcmp(patients[i].status, "treated")) != 0 && strcmp(patients[i].svNr, "")!=0 && strcmp(patients[i].arrival_method,"Other")==0){
            fprintf(fpointer_other,"%s,%s,%s,%s,%s,%s,%s,%lld,%s;\n",
                    patients[i].svNr, patients[i].vorname, patients[i].nachname,
                    patients[i].gebdat,patients[i].telefon, patients[i].email,
                    patients[i].arrival_method,patients[i].date_of_arrival, patients[i].status);
        }
    }
    fclose(fpointer_other);

    FILE *fpointer_ambulance = fopen("patients/patients_ambulance.txt","w");

    for (int i = 0; i < MAX_PAT; ++i) {
        if ((strcmp(patients[i].status, "treated")) != 0 && strcmp(patients[i].svNr, "")!=0 && strcmp(patients[i].arrival_method,"Ambulance")==0){
            fprintf(fpointer_ambulance,"%s,%s,%s,%s,%s,%s,%s,%lld,%s;\n",
                    patients[i].svNr, patients[i].vorname, patients[i].nachname,
                    patients[i].gebdat,patients[i].telefon, patients[i].email,
                    patients[i].arrival_method,patients[i].date_of_arrival, patients[i].status);
        }
    }
    fclose(fpointer_ambulance);

    FILE *fpointer_er= fopen("er.txt","w");

    for (int i = 0; i < MAX_ER; ++i) {
        fprintf(fpointer_er,"%d,%s;\n",
                er[i].room, er[i].status);
    }
    fclose(fpointer_er);
}

void query_seat_neighbour(Neighbour neigh[MAX_NEIGH], Register reg[MAX_REG], Seat seats[MAX_SEAT]){

    for (int i = 0; i < MAX_REG; ++i) {
        strcpy(neigh[i].ID, reg[i].pID);
        strcpy(neigh[i].n1, "-");
        strcpy(neigh[i].n2, "-");
        strcpy(neigh[i].n3, "-");
        strcpy(neigh[i].n4, "-");

        // Check left neighbor
        if (i > 0 && reg[i].seat_row == reg[i-1].seat_row && reg[i-1].seat_spot == reg[i].seat_spot - 1) {
            // Check time window overlap
            if (reg[i].start_time <= reg[i-1].end_time && reg[i-1].start_time <= reg[i].end_time) {
                strcpy(neigh[i].n1, reg[i-1].pID);
            }
        }

        // Check right neighbor
        if (i < MAX_REG - 1 && reg[i].seat_row == reg[i+1].seat_row && reg[i+1].seat_spot == reg[i].seat_spot + 1) {
            // Check time window overlap
            if (reg[i].start_time <= reg[i+1].end_time && reg[i+1].start_time <= reg[i].end_time) {
                strcpy(neigh[i].n2, reg[i+1].pID);
            }
        }

        // Check if left neighbor's seat is occupied by a different patient
        if (i > 0 && reg[i].seat_row == reg[i-1].seat_row && reg[i-1].seat_spot != reg[i].seat_spot - 1) {
            strcpy(neigh[i].n3, reg[i-1].pID);
        }

        // Check if right neighbor's seat is occupied by a different patient
        if (i > 0 && reg[i].seat_row == reg[i+1].seat_row && reg[i+1].seat_spot != reg[i].seat_spot + 1) {
            strcpy(neigh[i].n4, reg[i+1].pID);
        }
    }

    FILE *fpointer_neighbours= fopen("neighbours.txt","w");
    int k=0;
   while (strcmp(neigh[k].ID,"")!=0){
        fprintf(fpointer_neighbours,"%s,%s,%s,%s,%s;\n",
                neigh[k].ID,neigh[k].n1,neigh[k].n2,neigh[k].n3,neigh[k].n4);
        k++;
    }
    fclose(fpointer_neighbours);
    printf("done!");
}

void contact_trace(Neighbour neigh[MAX_NEIGH], Register reg[MAX_REG], Seat seats[MAX_SEAT]){
    int flag=1;
    char input[11];
    int fail_counter=0;

    while (flag==1){
        flag=0;
        printf("Enter Social Security Number of Patient: ");
        scanf("%11s",input);
        for (int i = 0; i < MAX_NEIGH; ++i) {
            if (strcmp(neigh[i].ID,input)==0){
                printf("%s had contact with following people:\n%s\n%s\n%s\n%s",
                       neigh[i].ID,neigh[i].n1,neigh[i].n2,neigh[i].n3,neigh[i].n4);
                flag=0;
                break;
            } else fail_counter++;
        }
        if (fail_counter>=MAX_NEIGH){
            printf("Error!\n");
            flag=1;
        }
    }
}



int main() {

    Register reg[MAX_REG];
    Seat seats[MAX_SEAT];
    Patient patients[MAX_PAT];
    ER er[MAX_ER];
    Neighbour neigh[MAX_NEIGH];

    int read;
    int reg_record=0;
    int patient_record=0;
    char input='d';

    while (input!=0){

        load_er_data(er);
        load_seat_data(seats);
        load_patient_data(patients,patient_record);
        load_register(reg, seats, reg_record);


        printf("[1] Patients\n"
               "[2] Seats\n"
               "[3] Assign Patient a seat\n"    //Ask if patient wants to do it automatically or manually
               "[4] Send Patient to the ER\n"   //Ask if patient wants to do it automatically or manually
               "[5] Discharge Patient\n"
               "[6] Contact trace\n\n"
               "[0] End Program\n");
        scanf("%c",&input);

        int i=0;
        char svnr[11];

        switch (input) {
            case '1':
                input = '0';
                printf("[1] Show Patient records \n[2] Add new Patient \n[3] Search for Patient\n");
                scanf(" %c", &input); // Notice the space before %c to consume the newline character
                switch (input) {
                    case '1':
                        while (strcmp(patients[i].svNr, "") != 0) {
                            // Print patient information
                            printf("Social security number: %s\n"
                                   "First Name: %s\n"
                                   "Last Name: %s\n"
                                   "Date of Birth: %s\n"
                                   "Telephone Number: %s\n"
                                   "Email address: %s\n"
                                   "Method of Arrival: %s\n"
                                   "Date of arrival: %s"
                                   "Status: %s\n",
                                   patients[i].svNr, patients[i].vorname, patients[i].nachname, patients[i].gebdat,
                                   patients[i].telefon, patients[i].email, patients[i].arrival_method,
                                   ctime(&patients[i].date_of_arrival), patients[i].status);
                            printf("-----------------------------\n");
                            i++;
                        }
                        break;
                    case '2':
                        add_patient(patients, patient_record);
                        break;
                    case '3':
                        printf("Social Security Number of Patient: ");
                        scanf("%11s",svnr);
                        int fail=0;
                        for (int j = 0; j < MAX_PAT; ++j) {
                            if (strcmp(svnr,patients[j].svNr)==0){
                                printf("Social security number: %s\n"
                                       "First Name: %s\n"
                                       "Last Name: %s\n"
                                       "Date of Birth: %s\n"
                                       "Telephone Number: %s\n"
                                       "Email address: %s\n"
                                       "Method of Arrival: %s\n"
                                       "Date of arrival: %s"
                                       "Status: %s\n",
                                       patients[j].svNr, patients[j].vorname, patients[j].nachname, patients[j].gebdat,
                                       patients[j].telefon, patients[j].email, patients[j].arrival_method,
                                       ctime(&patients[j].date_of_arrival), patients[j].status);
                                break;
                            } else fail++;
                        }
                        if (fail>=MAX_PAT){
                            printf("No matches found!\n");
                        }
                        break;
                    default:
                        printf("Invalid input!\n");
                        break;
                }
                sleep(5);
                getchar();
                break;

            case '2':
                input = '0';
                printf("[1] Show seats \n[2] Free all seats\n");
                scanf(" %c", &input); // Notice the space before %c to consume the newline character
                switch (input) {
                    case '1':
                        for (int j = 0; j < MAX_SEAT; ++j) {
                            // Print seat information
                            printf("Row: %d - Seat: %d - Occupied by : %s\n",
                                   seats[j].row, seats[j].spot, seats[j].status);
                        }
                        break;
                    case '2':
                        reset_seats(seats,patients);
                        break;
                    default:
                        printf("Invalid input!\n");
                        break;
                }
                sleep(5);
                getchar();
                break;
            case '3':
                assign_patient_to_seat(seats, reg, patients, reg_record);
                sleep(5);
                getchar();
                break;

            case '4':
                send_patient_to_er(seats, reg, patients, er);
                query_seat_neighbour(neigh, reg, seats);
                sleep(5);
                getchar();
                break;

            case '5':
                successful_treatment(patients, er);
                sleep(5);
                getchar();
                break;

            case '6':
                contact_trace(neigh,reg,seats);
                sleep(5);
                getchar();
                break;

            case '0':
                return -1;
                break;

            default:
                printf("Invalid input!\n");
                break;
        }

    }
    return 0;
}
