#include <stdio.h>
#include <time.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>

#define MAX_REG 100
#define MAX_SEAT 25
#define MAX_PAT 100

typedef struct{
    char svNr[11];
    char vorname[20];
    char nachname[20];
    char gebdat[20];
    char telefon[20];
    char email[40];
    char arrival_method[20];
    char status[20];
}Patient;

typedef struct{
    //char id[5];
    int row;
    int spot;
    char status[20];
}Seat;

typedef struct {
    //int regID;
    char pID[11];
    //char seat[MAX_SEAT];
    int seat_row;
    int seat_spot;
    time_t start_time;
    time_t end_time;
}Register;

void reset_seats(Seat seats[MAX_SEAT]){
    for (int i = 0; i < 5; ++i) {
        for (int j = 0; j < 5; ++j) {
            int index = i * 5 + j;
            seats[index].row = i+1;
            seats[index].spot = j+1;
        }
    }
    FILE *fpointer_seat;
    fpointer_seat= fopen("seats.txt","w");
    for (int i = 0; i < MAX_SEAT; ++i) {
        fprintf(fpointer_seat,"%d,%d,Free;\n",
                seats[i].row,seats[i].spot);
    }
    fclose(fpointer_seat);
}

int read_register(Register reg[MAX_REG], Seat seats[MAX_SEAT], int record){
    FILE *fpointer_register;
    fpointer_register= fopen("register.txt","r");

    int read;
    record=0;


    do {
        char seat_buffer[30], start_time_buffer[30], end_time_buffer[30], row_buffer, spot_buffer;
        time_t start, end;
        struct tm tm_start,tm_end;
        const char* month_name[] = {
                "Jan", "Feb", "Mar", "Apr", "May", "Jun",
                "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"
        };

        read= fscanf(fpointer_register, "%29[^,],%c,%c,%29[^,],%29[^;]",
                     reg[record].pID, &row_buffer, &spot_buffer, start_time_buffer, end_time_buffer);

        reg[record].seat_row = row_buffer - '0';
        reg[record].seat_spot = spot_buffer - '0';

        sscanf(start_time_buffer, "%*s %*s %d %d:%d:%d %d", &tm_start.tm_mday, &tm_start.tm_hour, &tm_start.tm_min, &tm_start.tm_sec, &tm_start.tm_year);
        tm_start.tm_year -= 1900; // tm_year is years since 1900
        tm_start.tm_mon = 0;
        while (strncmp(month_name[tm_start.tm_mon], start_time_buffer + 4, 3) != 0) {
            tm_start.tm_mon++;
        }

        sscanf(end_time_buffer, "%*s %*s %d %d:%d:%d %d", &tm_end.tm_mday, &tm_end.tm_hour, &tm_end.tm_min, &tm_end.tm_sec, &tm_end.tm_year);
        tm_end.tm_year -= 1900; // tm_year is years since 1900
        tm_end.tm_mon = 0;
        while (strncmp(month_name[tm_end.tm_mon], end_time_buffer + 4, 3) != 0) {
            tm_end.tm_mon++;
        }

        reg[record].start_time = mktime(&tm_start);
        reg[record].end_time = mktime(&tm_end);


        if (read==5) record++;
        else printf("Formatting error");

        if (ferror(fpointer_register)) {
            printf("Error opening file!");
            return 1;
        }

    } while (!feof(fpointer_register));

    /*for (int i = 0; i < record; ++i) {
        printf("%s, %s, %lld, %lld\n", reg[i].pID, reg[i].seat, reg[i].start_time, reg[i].end_time);
    }*/


    printf("Register.txt opened successfully!\n\n");
    fclose(fpointer_register);

    return 0;
}

void load_patient_data(Patient patients[MAX_PAT], int patient_record){

    FILE *fpointer_patient= fopen("patients.txt","r");
    int read;
    patient_record=0;

    do {
        read= fscanf(fpointer_patient,"%50[^,],%50[^,],%50[^,],%20[^,],%50[^,],%50[^,],%50[^,],%50[^;\n]%*[\n;]",
                     patients[patient_record].svNr,patients[patient_record].vorname,patients[patient_record].nachname,
                     patients[patient_record].gebdat,patients[patient_record].telefon,patients[patient_record].email, patients[patient_record].arrival_method, patients[patient_record].status);

        if(read==8) patient_record++;
        else printf("Formatting error! (patients.txt) read=%d",read);

    } while (!feof(fpointer_patient));
    fclose(fpointer_patient);

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

//add function to check if Social-Security number exist
void assign_patient_to_seat(Seat seats[MAX_SEAT], Register reg[MAX_REG], Patient patients[MAX_PAT], int record){

    //read_register(reg, seats, record);

    int occupied_counter=0;
    int seat_flag=0;
    int fail_counter=0;

    for (int i = 0; i < MAX_SEAT; ++i) {
        if(strcmp(seats[i].status,"Occupied")!=0){                  //check for seat that isn't occupied

            while (seat_flag==0) {
                printf("Social security number of patient: ");
                scanf("%10s", reg[record].pID);

                for (int j = 0; j < MAX_PAT; ++j) {
                    if (strcmp(reg[record].pID, patients[j].svNr) == 0) {

                        if (strcmp(patients[j].status, "Seating") == 0) {
                            printf("Patient is already seated!\n");
                            seat_flag=0;
                        } else {
                            seat_flag = 1;
                            strcpy(patients[j].status, "Seating");
                        }
                    } else fail_counter++;
                }
                if (fail_counter>=MAX_PAT){
                    printf("There is no patient with this social security number!\n");
                    seat_flag=0;
                }

            }
            strcpy(seats[i].status,"Occupied");

            reg[record].start_time = time(NULL);
            reg[record].end_time = time(NULL);

            FILE *fpointer_reg;
            char *string_start= ctime(&reg[record].start_time);
            char *string_end= ctime(&reg[record].end_time);

            fpointer_reg= fopen("register.txt", "a");

            if (fpointer_reg == NULL){
                printf("Datei nicht gefunden");
            }

            fprintf(fpointer_reg, "%s,%d,%d,%s,%s\n",
                    reg[record].pID, seats[i].row, seats[i].spot, string_start, string_end);

            printf("Patient: %s has been asssigned seat R%dS%d at %s\n",reg[record].pID,seats[i].row,seats[i].spot,string_start);

            fclose(fpointer_reg);
            break;
        } else occupied_counter++;
    }
    if (occupied_counter>=MAX_SEAT)
        printf("all seats are currently occupied!");

    //Overwriting existing seats.txt file with new data
    FILE *fpointer_seats;
    fpointer_seats= fopen("seats.txt","w");

    for (int i = 0; i < MAX_SEAT; ++i) {
        fprintf(fpointer_seats,"%d,%d,%s;\n",seats[i].row,seats[i].spot,seats[i].status);
    }

    fclose(fpointer_seats);

    //Overwriting existing patient.txt file with new data
    FILE *fpointer_patient= fopen("patients.txt","w");

    int patient_record=0;

    while (strcmp(patients[patient_record].svNr,"")!=0){
        fprintf(fpointer_patient,"%s,%s,%s,%s,%s,%s,%s,%s;\n",
                patients[patient_record].svNr,patients[patient_record].vorname,patients[patient_record].nachname,patients[patient_record].gebdat,
                patients[patient_record].telefon,patients[patient_record].email, patients[patient_record].arrival_method, patients[patient_record].status);
        patient_record++;
    }
    fclose(fpointer_patient);
}

//add function to check if Social-Security number exist
void send_patient_to_er(Seat seats[MAX_SEAT], Register reg[MAX_REG]){

    int record =0;
    read_register(reg, seats, record);

    char input[10];

    printf("Starttime: %s\nEndtime: %s\n",ctime(&reg[0].start_time),ctime(&reg[0].end_time));

    printf("Social security number of patient: ");
    scanf("%10s", input);

    for (int i = 0; i < MAX_REG; ++i) {
        if(strcmp(input, reg[i].pID) == 0 && reg[i].start_time == reg[i].end_time){
            reg[i].end_time= 1706224000;
            break;
        }
    }
    //Overwrite existing register.txt file with new data
    FILE *fpointer_reg= fopen("register.txt","w");
    time_t t = 1695224000;

    for (int i = 0; i < 1; ++i) {
        char *string_start= ctime(&reg[i].start_time);
        //reg[i].end_time= time(NULL);
        char *string_end= ctime(&reg[i].end_time);

        fprintf(fpointer_reg, "%s,%d,%d,%s,%s\n",
                reg[i].pID, reg[i].seat_row, reg[i].seat_spot, string_start, string_end);
    }
    fclose(fpointer_reg);


    printf("\nPrüfen:\n");
    printf("Starttime: %s\nEndtime: %s\n",ctime(&reg[0].start_time),ctime(&reg[0].end_time));

}



void add_patient(Patient patients[MAX_PAT], int patient_record){
    FILE *fpointer_patient;

    printf("Social security number: ");
    scanf("%11s",patients[patient_record].svNr);

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

    printf("\nArrival Method (Ambulance/Other): ");
    scanf("%20s",patients[patient_record].arrival_method);

    strcpy(patients[patient_record].status,"-");

    fpointer_patient = fopen("patients.txt","a");

    if(fpointer_patient==NULL){
        printf("Couldn't find file ---> creating new file\n");
    }

    fprintf(fpointer_patient,"%s,%s,%s,%s,%s,%s,%s,%s;",
            patients[patient_record].svNr,patients[patient_record].vorname,patients[patient_record].nachname,patients[patient_record].gebdat,
            patients[patient_record].telefon,patients[patient_record].email, patients[patient_record].arrival_method, patients[patient_record].status);

    fclose(fpointer_patient);

    printf("\nSocial security number: %s\n"
           "First Name: %s\n"
           "Last Name: %s\n"
           "Date of Birth: %s\n"
           "Telephone Number: %s\n"
           "Email address: %s\n"
           "Method of Arrival: %s\n"
           "Status: %s\n",
           patients[patient_record].svNr,patients[patient_record].vorname,patients[patient_record].nachname,patients[patient_record].gebdat,
           patients[patient_record].telefon,patients[patient_record].email, patients[patient_record].arrival_method, patients[patient_record].status);

    printf("\nData successfully saved!");
}

int main() {

    Register reg[MAX_REG];
    Seat seats[MAX_SEAT];
    Patient patients[MAX_PAT];

    int read;
    int reg_record=0;
    int patient_record=0;

    //reset_seats(seats);

    load_seat_data(seats);
    load_patient_data(patients,patient_record);
    assign_patient_to_seat(seats,reg,patients,reg_record);
    //read_register(reg,seats);
    //send_patient_to_er(seats,reg);
    //add_patient(patients,patient_record);


    return 0;
}
