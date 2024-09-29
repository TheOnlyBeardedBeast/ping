const int numbers[] = {
    0b0111111, // 0
    0b0000110, // 1
    0b1011011, // 2
    0b1001111, // 3
    0b1100110, // 4
    0b1101101, // 5
    0b1111101, // 6
    0b0000111, // 7
    0b1111111, // 8
    0b1101111  // 9
};

const byte adresses[2] = {0xf1, 0xf0};

class ScoreBoard
{
private:
    byte scores[2] = {0, 0};

public:
    byte getScore(size_t player)
    {
        if (player > 1)
        {
            return 0;
        }

        return scores[player];
    }

    void setScore(size_t player, byte score)
    {
        if (player > 1)
        {
            return;
        }

        scores[player] = score;

        renderPlayerScore(player);
    }

    void incrementScore(size_t player)
    {
        if (player > 1)
        {
            return;
        }

        scores[player] = scores[player] + 1;

        renderPlayerScore(player);
    }

    void resetScore()
    {
        scores[0] = 0;
        scores[1] = 0;

        Serial3.write(0x80);
        Serial3.write(0x89);
        Serial3.write(0xff);
        Serial3.write(numbers[0]);
        Serial3.write(0x8f);
    }

    bool checkForWinner()
    {
        if (scores[0] == 5 || scores[1] == 5)
        {
            return true;
        }

        return false;
    }

private:
    void renderPlayerScore(size_t player)
    {
        Serial3.write(0x80);
        Serial3.write(0x89);
        Serial3.write(adresses[player]);
        Serial3.write(numbers[scores[player]]);
        Serial3.write(0x8f);
    }
};