#include <cassert>
#include <stdexcept> // Для std::logic_error

// Умный указатель, удаляющий связанный объект при своём разрушении.
// Параметр шаблона T задаёт тип объекта, на который ссылается указатель
template <typename T>
class ScopedPtr {
public:
    // Конструктор по умолчанию создаёт нулевой указатель,
    // так как поле ptr_ имеет значение по умолчанию nullptr
    ScopedPtr() = default;

    // Создаёт указатель, ссылающийся на переданный raw_ptr.
    // raw_ptr ссылается либо на объект, созданный в куче при помощи new,
    // либо является нулевым указателем
    // Спецификатор noexcept обозначает, что метод не бросает исключений
    explicit ScopedPtr(T* raw_ptr) noexcept : ptr_(raw_ptr) {
        // Ничего больше не нужно - автоматическая инициализация ptr_
    }

    // Удаляем у класса конструктор копирования
    ScopedPtr(const ScopedPtr&) = delete;

    // Деструктор. Удаляет объект, на который ссылается умный указатель.
    ~ScopedPtr() {
        delete ptr_; // Удаляем объект, на который указывает ptr_
    }

    // Возвращает указатель, хранящийся внутри ScopedPtr
    T* GetRawPtr() const noexcept {
        return ptr_; // Просто возвращаем значение ptr_
    }

    // Прекращает владение объектом, на который ссылается умный указатель.
    // Возвращает прежнее значение "сырого" указателя и устанавливает поле ptr_ в null
    T* Release() noexcept {
        T* temp = ptr_; // Сохраняем текущий указатель
        ptr_ = nullptr; // Устанавливаем ptr_ в nullptr, чтобы прекратить владение
        return temp; // Возвращаем сохранённый указатель
    }

    // Преобразование к типу bool для проверки на nullptr
    explicit operator bool() const noexcept {
        return ptr_ != nullptr; // Возвращает true, если ptr_ не равен nullptr
    }

    // Оператор разыменования
    T& operator*() const {
        if (!ptr_) {
            throw std::logic_error("Dereferencing a null pointer");
        }
        return *ptr_; // Возвращаем разыменованный указатель
    }

    // Оператор доступа к полям класса
    T* operator->() const {
        if (!ptr_) {
            throw std::logic_error("Accessing a member of a null pointer");
        }
        return ptr_; // Возвращаем указатель
    }

private:
    T* ptr_ = nullptr; // Указатель на объект, которым владеет ScopedPtr
};

// Этот main тестирует класс ScopedPtr
int main() {
    // Вспомогательный "шпион", позволяющий узнать о своём удалении
    struct DeletionSpy {
        explicit DeletionSpy(bool& is_deleted)
            : is_deleted_(is_deleted) {
        }
        ~DeletionSpy() {
            is_deleted_ = true;
        }
        bool& is_deleted_;
    };

    // Проверяем автоматическое удаление
    {
        bool is_deleted = false;
        {
            // настраиваем "шпион", чтобы при своём удалении он выставил is_deleted в true
            DeletionSpy* raw_ptr = new DeletionSpy(is_deleted);
            ScopedPtr<DeletionSpy> p(raw_ptr);
            assert(p.GetRawPtr() == raw_ptr);
            assert(!is_deleted);
            // При выходе из блока деструктор p должен удалить "шпиона"
        }
        // Если деструктор умного указателя работает правильно, шпион перед своей "смертью"
        // должен выставить is_deleted в true
        assert(is_deleted);
    }

    // Проверяем работу метода Release
    {
        bool is_deleted = false;
        DeletionSpy* raw_ptr = new DeletionSpy(is_deleted);
        {
            ScopedPtr<DeletionSpy> scoped_ptr(raw_ptr);
            assert(scoped_ptr.Release() == raw_ptr);
            assert(scoped_ptr.GetRawPtr() == nullptr);
            // После Release умный указатель не ссылается на объект и не удаляет его при своём удалении
        }
        assert(!is_deleted);
        delete raw_ptr;
        assert(is_deleted);
    }

    // Проверяем работу операторов разыменования и доступа к членам
    {
        bool is_deleted = false;
        DeletionSpy* raw_ptr = new DeletionSpy(is_deleted);
        ScopedPtr<DeletionSpy> p(raw_ptr);
        assert(static_cast<bool>(p)); // Проверка, что указатель не нулевой
        assert(p.GetRawPtr() == raw_ptr);
        
        // Проверка разыменования
        assert(&(*p) == raw_ptr);
        
        // Проверка доступа к члену через оператор->
        assert(p->is_deleted_ == false); // Проверка, что член указан верно
    }

    // Проверка выброса исключений
    {
        ScopedPtr<DeletionSpy> p;
        try {
            *p; // Должно выбросить исключение
        } catch (const std::logic_error& e) {
            assert(std::string(e.what()) == "Dereferencing a null pointer");
        }
        
        try {
            p->is_deleted_; // Должно выбросить исключение
        } catch (const std::logic_error& e) {
            assert(std::string(e.what()) == "Accessing a member of a null pointer");
        }
    }

    return 0;
}