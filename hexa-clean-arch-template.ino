#include <LiquidCrystal.h>

// ------------------------------------
//         DECLARACIONES ANTICIPADAS
// ------------------------------------

class User;
class IUserRepository;
class IController; 
class ILCDOutput; 
class IUserCycleService;
class UserCycleService;
class SimulatedUserRepository;

// ====================================
//          CAPA DE DOMINIO 🧠
// ====================================

class User {
private:
    const int userId;
    const char* const name;
    
public:
    User(int userId, const char* name)
        : userId(userId), name(name) {} 

    int getUserId() const { return userId; }
    const char* getName() const { return name; }
};

class IUserRepository {
public:
    virtual ~IUserRepository() = default;
    virtual User findById(int searchId) const = 0;
    virtual int getMaxUsers() const = 0;
};

class IController {
public:
    virtual ~IController() = default;
    virtual void begin(int cols, int rows) = 0;
    virtual void update() = 0;
};

class ILCDOutput {
public:
    virtual ~ILCDOutput() = default;
    virtual void init(int cols, int rows) = 0;
    virtual void displayUser(const User& user) = 0;
    virtual void displayMessage(const char* message) = 0;
};

class IUserCycleService {
public:
    virtual ~IUserCycleService() = default;
    virtual User getNextUser() = 0;
};

// ====================================
//    CAPA DE SERVICIOS DE APLICACIÓN ✨
// ====================================

class UserCycleService : public IUserCycleService {
private:
    IUserRepository* userRepo;
    int currentId = 0;
    const int maxUsers;
public:
    UserCycleService(IUserRepository* repo)
        : userRepo(repo), maxUsers(repo->getMaxUsers()) {} 

    User getNextUser() override {
        currentId = (currentId % maxUsers) + 1;
        return userRepo->findById(currentId);
    }
};

// ====================================
//        CAPA DE INFRAESTRUCTURA 💾
// ====================================

class SimulatedUserRepository : public IUserRepository {
private:
    static constexpr int MAX_USERS = 3;
    User users[MAX_USERS] = {
        User(1, "OSCAR GONZALEZ"),
        User(2, "MARIA LOPEZ"),
        User(3, "PEDRO PEREZ")
    };
public:
    User findById(int searchId) const override {
        for (int i = 0; i < MAX_USERS; ++i) {
            if (users[i].getUserId() == searchId) {
                return users[i];
            }
        }
        return User(0, "Not Found"); 
    }
    int getMaxUsers() const override { return MAX_USERS; }
};

class LCDAdapter : public ILCDOutput {
private:
    LiquidCrystal lcd; 
public:
    LCDAdapter(int rs, int en, int d4, int d5, int d6, int d7) 
        : lcd(rs, en, d4, d5, d6, d7) {}
    void init(int cols, int rows) override { lcd.begin(cols, rows); }
    void displayMessage(const char* message) override { lcd.clear(); lcd.print(message); }
    void displayUser(const User& user) override {
        lcd.setCursor(0, 1);
        lcd.print("ID:");
        lcd.print(user.getUserId());
        lcd.print(" Name:");
        lcd.print(user.getName());
        int cursorPos = 6 + String(user.getUserId()).length() + 6 + String(user.getName()).length();
        for (int i = cursorPos; i < 16; ++i) {
            lcd.print(" ");
        }
    }
};

// ====================================
//           CAPA DE APLICACIÓN ⚙️
// ====================================

class Controller : public IController {

private:
    ILCDOutput* lcdOutput;
    IUserCycleService* cycleService;
    
    unsigned long lastUpdateTime = 0;
    const long updateInterval = 1000;

public:
    Controller(ILCDOutput* output, IUserCycleService* service)
        : lcdOutput(output), 
          cycleService(service)
    {}

    void begin(int cols, int rows) override
    {
        lcdOutput->init(cols, rows); 
        lcdOutput->displayMessage("System Ready!");
    }

    void update() override
    {
        if (millis() - lastUpdateTime >= updateInterval)
        {
            lastUpdateTime = millis();
            User user = cycleService->getNextUser(); 
            lcdOutput->displayUser(user); 
        }
    }
};

// ------------------------------------
//      INICIALIZACIÓN DE ARDUINO ⚡
// (Composition Root)
// ------------------------------------

LCDAdapter lcd_adapter(12, 11, 5, 4, 3, 2);
SimulatedUserRepository repository;
UserCycleService concrete_service(&repository); 
Controller concrete_control(&lcd_adapter, &concrete_service); 
IController* control = &concrete_control;

void setup()
{
    control->begin(16, 2);
}

void loop()
{
    control->update();
}
