<?php
// $Id$
/**
 * @file
 */

function calais_call_test() {
  
  $data = calais_getTestContent();
  
  $body = filter_xss($data);
	$date = format_date(time(), 'custom', 'r');
	$request = calais_api_build_xml_request('Calais Call Test', $date, $body);
	$keywords = calais_api_analyze($request, array('contentType' => 'TEXT/XML'));
  
  $rawmetadata .= "<pre>";
  $rawmetadata .= print_r($keywords, TRUE);
  $rawmetadata .= "</pre>";
  
  return $rawmetadata;
}

function calais_getTestContent() {
  
  return <<<EOD
Democratic politicians should have three dates circled on their calendars: April 22, May 6 and July 1. Each represents a potential turning point in the nomination battle between Barack Obama and Hillary Clinton.

April 22, the day of the Pennsylvania primary, is Clinton's next change to make a strong case for staying in the race to the end. Conversely, it is a critical opportunity for Obama to show that he continues to expand his appeal within the party, as someone who hopes to become the nominee must do.

May 6 is when North Carolina and Indiana hold their primaries. It has gained significance over the past two weeks as a potential make-or-break day for both candidates, perhaps the only date between now and the end of the primaries that could deliver a seismic jolt to the trajectory of the nomination battle.

July 1 is Democratic National Committee chairman Howard Dean's deadline for superdelegates to get off the fence and declare whether they will vote for Clinton or Obama.

Start with Pennsylvania. Nothing underscores the significance of the Keystone State's primary more than the current bus tour that Obama is taking from one side of the state to the other.

After Obama lost to Clinton in Ohio by 10 percentage points, there were questions about how hard he would play in Pennsylvania. Clinton has family roots in the state. She has the support of Gov. Edward Rendell and Philadelphia Mayor Michael Nutter. The state's demographic profile fits Clinton almost perfectly.

The polls offer a bleak landscape for Obama. Clinton has enjoyed a double-digit lead in the state for months. Obama is braced for a loss in Pennsylvania but his campaign team has concluded he cannot afford another loss of the magnitude of Ohio.

Obama has retooled his campaign in Pennsylvania, bringing in Paul Tewes as his new state director. Tewes was Obama's state director in Iowa and later in Ohio. He has seen what worked and what didn't and he has the full trust of Obama's Chicago headquarters team.

More telling are the changes in Obama's campaign style. He is more focused on bread-and-butter issues and is spending more time in informal settings with working people. He went bowling -- for the first time in decades -- over the weekend and has shown up in cafes and bars in an effort to connect with working-class Pennsylvanians.

His hope in Pennsylvania is something often heard during the early stages of a presidential campaign -- to exceed expectations. That's an odd objective for someone who purports to be the Democratic front-runner, but he is seeking to define down his prospects in the hope that a relatively narrow loss can be spun into victory.

Clinton needs Ohio plus -- an outsized victory that silences for a time any talk that she should get out. As Ohio showed, there is nothing like a decisive victory to change the conversation.

Clinton spent the weekend beating back such talk after Vermont Sen. Patrick Leahy had publicly called for her to get out. On Saturday, she made an emphatic statement to the Post's Perry Bacon Jr. that she's taking her fight all the way to Denver and that she will continue to press for a resolution to the problem of Michigan and Florida. Her husband urged Democrats to chill, to let the race play out according to the calendar and not attempt to disenfranchise the voters in the remaining 10 contests.

May 6 looms as an even bigger moment. Nobody expects a significant change in the race before Pennsylvania and, given Clinton's prospects of victory there, Obama loyalists may have a harder time immediately after that primary to argue that she should quit.

That could leave it to voters in North Carolina and Indiana to change the status quo. Obama is favored in North Carolina and The Wall Street Journal reported Monday morning that the entire congressional delegation is moving to endorse him. If he wins Indiana as well, a state that fits neither candidate perfectly, he will then argue that he has broken through. At that point, Clinton would face real pressure.

But Obama has let these opportunities slip away before. Should Clinton manage to win both Indiana and North Carolina, she and Obama will be in slugfest through the rest of the campaign. If Democrats have been worried about some of the attacks going back and forth between the two campaigns in the past two weeks, they should expect even worse at that point.

Any kind of muddled conclusion to May 6 virtually assures that the race goes through to the end. That will leave it in that hands of the superdelegates, and it was to them that Dean was directing his comments last week.

By nature, many of these superdelegates will not be profiles in courage. Obama is doing is best to pry some of them loose. If he continues to do so over the next few weeks, and performs well in the upcoming primaries, he will gradually consolidate the nomination.

Whether Dean has the clout to push the superdelegates to a conclusion by July 1, if the primaries end with the race notably closer in delegates and popular vote than it is today, is a real question. He may need the help of other senior Democrats who have not previously taken sides.

Al Gore, on CBS's "60 Minutes" Sunday, laughed when asked by correspondent Lesley Stahl whether he would step in to try to resolve the nomination. "I'm not applying for the job of broker," he said. Perhaps Gore still harbors a smidgen of hope that the party might eventually turn to him, not to broker a deal, but to become the nominee to unite a party fractured by the Obama-Clinton duel.

Depending on what happens on April 22, May 6 and by July 1, Democrats will know whether they have their nominee, or whether they are truly in need of either a broker or a uniter

EOD;
}
