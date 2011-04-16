#define BOOST_TEST_DYN_LINK

#include <boost/test/unit_test.hpp>
#include <powertabdocument/staff.h>
#include <powertabdocument/position.h>
#include <powertabdocument/tuning.h>
#include <powertabdocument/keysignature.h>

// Provides a staff with some positions already inserted
struct StaffFixture
{
    StaffFixture()
    {
        highPositions.push_back(new Position(0, 4, 0));
        highPositions.push_back(new Position(2, 4, 0));
        highPositions.push_back(new Position(4, 4, 0));
        highPositions.push_back(new Position(5, 4, 0));
        highPositions.push_back(new Position(6, 4, 0));

        // put some sample notes at each position
        highPositions.at(0)->InsertNote(new Note(1, 4));
        highPositions.at(0)->InsertNote(new Note(2, 3));
        highPositions.at(1)->InsertNote(new Note(1, 5));
        highPositions.at(2)->InsertNote(new Note(1, 5));
        highPositions.at(3)->InsertNote(new Note(1, 6));
        highPositions.at(4)->InsertNote(new Note(1, 5));

        lowPositions.push_back(new Position(1, 4, 0));
        lowPositions.push_back(new Position(3, 8, 0));

        staff.positionArrays[0] = highPositions;

        staff.positionArrays[1] = lowPositions;
    }

    ~StaffFixture() {}

    Staff staff;
    std::vector<Position*> highPositions;
    std::vector<Position*> lowPositions;
};

// Provides a blank staff
struct EmptyStaffFixture
{
    EmptyStaffFixture() {}
    ~EmptyStaffFixture() {}

    Staff staff;
};

BOOST_AUTO_TEST_SUITE(TestStaff)

    // tests for clef-related functions
    BOOST_AUTO_TEST_SUITE(Clef)

        BOOST_AUTO_TEST_CASE(IsValidClef)
        {
            BOOST_CHECK(Staff::IsValidClef(Staff::BASS_CLEF));
            BOOST_CHECK(Staff::IsValidClef(Staff::TREBLE_CLEF));
            BOOST_CHECK(!Staff::IsValidClef(Staff::BASS_CLEF + 1));
        }

        BOOST_FIXTURE_TEST_CASE(ModifyClef, EmptyStaffFixture)
        {
            staff.SetClef(Staff::BASS_CLEF);
            BOOST_CHECK_EQUAL(staff.GetClef(), Staff::BASS_CLEF);

            staff.SetClef(Staff::TREBLE_CLEF);
            BOOST_CHECK_EQUAL(staff.GetClef(), Staff::TREBLE_CLEF);
        }

    BOOST_AUTO_TEST_SUITE_END()

    BOOST_AUTO_TEST_SUITE(GetLastPosition)

        BOOST_AUTO_TEST_CASE(empty_staff)
        {
            Staff emptyStaff;
            BOOST_CHECK(emptyStaff.GetLastPosition() == NULL);
        }

        BOOST_AUTO_TEST_CASE(single_position)
        {
            Staff singleItemStaff;
            Position* pos1 = new Position();
            singleItemStaff.positionArrays[0].push_back(pos1);

            BOOST_CHECK_EQUAL(singleItemStaff.GetLastPosition(), pos1);
        }

        BOOST_FIXTURE_TEST_CASE(multiple_positions, StaffFixture)
        {
            BOOST_CHECK_EQUAL(staff.GetLastPosition(), highPositions.back());
        }

    BOOST_AUTO_TEST_SUITE_END()

    BOOST_FIXTURE_TEST_SUITE(CanHammerOn, StaffFixture)

        BOOST_AUTO_TEST_CASE(LessThanNextFretOkay)
        {
            Position* pos = highPositions.at(0);
            BOOST_CHECK(staff.CanHammerOn(pos, pos->GetNote(0)));
        }

        BOOST_AUTO_TEST_CASE(EqualFretsNotOkay)
        {
            Position* pos = highPositions.at(1);
            BOOST_CHECK(staff.CanHammerOn(pos, pos->GetNote(0)) == false);
        }

        BOOST_AUTO_TEST_CASE(GreaterThanNextFretNotOkay)
        {
            Position* pos = highPositions.at(3);
            BOOST_CHECK(staff.CanHammerOn(pos, pos->GetNote(0)) == false);
        }

        BOOST_AUTO_TEST_CASE(LastNoteCantBeHammeron)
        {
            Position* pos = highPositions.at(highPositions.size() - 1);
            BOOST_CHECK(staff.CanHammerOn(pos, pos->GetNote(0)) == false);
        }

    BOOST_AUTO_TEST_SUITE_END()

    /// Tests Staff::CanSlideBetweenNotes
    BOOST_FIXTURE_TEST_SUITE(CanSlideBetweenNotes, StaffFixture)

        BOOST_AUTO_TEST_CASE(DifferentFretOkay)
        {
            Position* pos = highPositions.at(0);
            BOOST_CHECK(staff.CanSlideBetweenNotes(pos, pos->GetNote(0)));

            pos = highPositions.at(3);
            BOOST_CHECK(staff.CanSlideBetweenNotes(pos, pos->GetNote(0)));
        }

        BOOST_AUTO_TEST_CASE(EqualFretsNotOkay)
        {
            Position* pos = highPositions.at(1);
            BOOST_CHECK(staff.CanSlideBetweenNotes(pos, pos->GetNote(0)) == false);
        }

        BOOST_AUTO_TEST_CASE(LastNoteCantSlideToNext)
        {
            Position* pos = highPositions.at(highPositions.size() - 1);
            BOOST_CHECK(staff.CanSlideBetweenNotes(pos, pos->GetNote(0)) == false);
        }

    BOOST_AUTO_TEST_SUITE_END()

    BOOST_FIXTURE_TEST_SUITE(GetSlideSteps, StaffFixture)

        BOOST_AUTO_TEST_CASE(Calculation)
        {
            Position* pos = highPositions.at(0);
            BOOST_CHECK_EQUAL(staff.GetSlideSteps(pos, pos->GetNote(0)), 1);

            pos = highPositions.at(1);
            BOOST_CHECK_EQUAL(staff.GetSlideSteps(pos, pos->GetNote(0)), 0);

            pos = highPositions.at(3);
            BOOST_CHECK_EQUAL(staff.GetSlideSteps(pos, pos->GetNote(0)), -1);
        }

        BOOST_AUTO_TEST_CASE(ThrowsWithNoAdjacentNote)
        {
            Position* pos = highPositions.at(0);
            // use note on the second string, since the next position only has a note on the first string
            BOOST_CHECK_THROW(staff.GetSlideSteps(pos, pos->GetNote(1)), std::logic_error);
        }

    BOOST_AUTO_TEST_SUITE_END()

    BOOST_FIXTURE_TEST_CASE(GetPositionCount, StaffFixture)
    {
        BOOST_CHECK_EQUAL(staff.GetPositionCount(0), 5u);
        BOOST_CHECK_EQUAL(staff.GetPositionCount(1), 2u);

        BOOST_CHECK_THROW(staff.GetPositionCount(2), std::out_of_range);
    }

    BOOST_FIXTURE_TEST_CASE(GetPosition, StaffFixture)
    {
        BOOST_CHECK_EQUAL(staff.GetPosition(0, 1), highPositions.at(1));
        BOOST_CHECK_EQUAL(staff.GetPosition(1, 1), lowPositions.at(1));

        BOOST_CHECK_THROW(staff.GetPosition(0, 99), std::out_of_range);
        BOOST_CHECK_THROW(staff.GetPosition(3, 0), std::out_of_range);
    }

    BOOST_FIXTURE_TEST_SUITE(GetPositionByPosition, StaffFixture)

        BOOST_AUTO_TEST_CASE(NullIfNoPositionFound)
        {
            BOOST_CHECK_EQUAL(staff.GetPositionByPosition(0, 1), (Position*)NULL);
            BOOST_CHECK_EQUAL(staff.GetPositionByPosition(0, 99), (Position*)NULL);

            BOOST_CHECK_THROW(staff.GetPositionByPosition(99, 0), std::out_of_range);
        }

        BOOST_AUTO_TEST_CASE(PositionFoundCorrectly)
        {
            BOOST_CHECK_EQUAL(staff.GetPositionByPosition(0, 2), highPositions.at(1));
            BOOST_CHECK_EQUAL(staff.GetPositionByPosition(1, 1), lowPositions.at(0));
        }

    BOOST_AUTO_TEST_SUITE_END()

    BOOST_FIXTURE_TEST_CASE(InsertPosition, StaffFixture)
    {
        const size_t originalPosCount = staff.GetPositionCount(0);
        Position* newPos = new Position(1, 4, 0);
        staff.InsertPosition(0, newPos);

        BOOST_CHECK_EQUAL(staff.GetPositionCount(0), originalPosCount + 1);
        // check that the position list was re-sorted by position index
        BOOST_CHECK_EQUAL(staff.GetPosition(0, 1), newPos);

        // cannot insert a position if there is already something at that position
        BOOST_CHECK(staff.InsertPosition(0, newPos) == false);
    }

    BOOST_FIXTURE_TEST_CASE(RemovePosition, StaffFixture)
    {
        staff.RemovePosition(0, 2);

        // check that it was removed, and the positions were shifted accordingly
        BOOST_CHECK_EQUAL(staff.GetPositionCount(0), highPositions.size() - 1);
        BOOST_CHECK_EQUAL(staff.GetPosition(0, 1), highPositions.at(2));
    }

    BOOST_FIXTURE_TEST_CASE(AssignmentAndEquality, StaffFixture)
    {
        Staff newStaff;
        BOOST_CHECK(newStaff != staff);

        newStaff = staff;
        BOOST_CHECK(newStaff == staff);

        newStaff.GetPosition(0, 2)->SetArpeggioDown(true);
        BOOST_CHECK(newStaff != staff);
    }
    
    BOOST_AUTO_TEST_CASE(GetNoteLocation)
    {
        Staff staff;
        
        KeySignature keySig;
        Tuning tuning;
        tuning.SetToStandard();
        
        Note note(0, 1);
        
        BOOST_CHECK_EQUAL(staff.GetNoteLocation(&note, &keySig, &tuning), 0);
        
        note.SetFretNumber(0);
        BOOST_CHECK_EQUAL(staff.GetNoteLocation(&note, &keySig, &tuning), 1); 
        
        note.SetFretNumber(3);
        BOOST_CHECK_EQUAL(staff.GetNoteLocation(&note, &keySig, &tuning), -1); 
        
        note.SetFretNumber(2);
        BOOST_CHECK_EQUAL(staff.GetNoteLocation(&note, &keySig, &tuning), 0); 
        
        note.SetFretNumber(14);
        BOOST_CHECK_EQUAL(staff.GetNoteLocation(&note, &keySig, &tuning), -7);
        
        note.SetFretNumber(21);
        BOOST_CHECK_EQUAL(staff.GetNoteLocation(&note, &keySig, &tuning), -11);
        
        // Bass clef
        staff.SetClef(Staff::BASS_CLEF);
        note.SetString(5);
        note.SetFretNumber(5);
        
        BOOST_CHECK_EQUAL(staff.GetNoteLocation(&note, &keySig, &tuning), 0);
        
        note.SetFretNumber(8);
        BOOST_CHECK_EQUAL(staff.GetNoteLocation(&note, &keySig, &tuning), -2);
        
        note.SetOctave15ma(true);
        BOOST_CHECK_EQUAL(staff.GetNoteLocation(&note, &keySig, &tuning), 12);
    }

BOOST_AUTO_TEST_SUITE_END()
