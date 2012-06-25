<?php

/**
 * @file
 * Rules integration for RedHen Engagements.
 */

/**
 * Implements hook_rules_action_info().
 */
function redhen_engagement_rules_action_info() {
  $defaults = array(
		'parameter' => array(
			'score' => array(
				'type' => 'list<integer>',
				'label' => t('Engagement Score'),
				'options list' => 'redhen_engagement_score_options',
			),
		),
    'group' => t('Engagement'),
    'access callback' => 'redhen_engagement_access',
  );
  if (module_exists('registration')) {
    $actions['score_registration_engagement']['parameter']['registration'] = array(
  		'type' => 'registration',
  		'label' => t('Registration'),
  	  'description' => t('The registration entity to be scored.'),
  	);
  	$actions['score_registration_engagement'] = $defaults + array(
      'label' => t('Score registration engagement'),
      'base' => 'redhen_engagement_rules_action_score_registration',
    );
  }
  if (module_exists('comment')) {
    $actions['score_comment_engagement'] = $defaults + array(
      'label' => t('Score comment engagement'),
      'base' => 'redhen_engagement_rules_action_score_comment',
    );
  	$actions['score_comment_engagement']['parameter']['comment'] = array(
  		'type' => 'comment',
  		'label' => t('Comment'),
  	  'description' => t('The comment entity to be scored.'),
  	); 
  }
  if (module_exists('webform_rules')) {
    $actions['score_webform_engagement'] = $defaults + array(
      'label' => t('Score webform engagement'),
      'base' => 'redhen_engagement_rules_action_score_webform',
    );
  	$actions['score_webform_engagement']['parameter']['webform'] = array(
  		'type' => 'webform',
  		'label' => t('Webform'),
  	  'description' => t('The webform submission to be scored. <em>Note: At this time scoring of anonymous webform submissions is not supported.</em>'),
  	); 
  }

  return $actions;
}

/**
 * Helper function for when the "registration engagement" Rule action is triggered.
 *
 * @TODO: This action depends on Registration, so it can't be located just in redhen_engagement.
 */
function redhen_engagement_rules_action_score_registration($score, $registration) {
	if (!empty($registration->anon_mail)) {
	  $contact = redhen_engagement_rules_action_find_by_mail($registration->anon_mail);
	}
	elseif (!empty($registration->user_uid)) {
	  $contact = redhen_engagement_rules_action_find_by_uid($registration->user_uid);
	}
	if ($contact) {
    redhen_enagement_rules_action_score_contact($score, $contact, 'Scored registration', 'registration', $registration->registration_id);
	}
}

/**
 * Helper function for when the "comment enagement" Rule action is triggered.
 *
 * @TODO: This action depends on Comment, so it can't be located just in redhen_engagement.
 */
function redhen_engagement_rules_action_score_comment($score, $comment) {
	if (!empty($comment->mail)) {
	  $contact = redhen_engagement_rules_action_find_by_mail($comment->mail);
	}
	elseif (!empty($comment->uid)) {
	  $contact = redhen_engagement_rules_action_find_by_uid($comment->uid);
	}
	if ($contact) {
    redhen_enagement_rules_action_score_contact($score, $contact, 'Scored comment', 'comment', $comment->cid);
	}
}

/**
 * Helper function for when the "webform enagement" Rule action is triggered.
 *
 * @TODO: This action depends on Webform Rules, so it can't be located just in redhen_engagement.
 */
function redhen_engagement_rules_action_score_webform($score, $webform) {
  $result = db_query('SELECT w.nid, w.uid FROM {webform_submissions} w WHERE w.sid = :sid', array(':sid' => $webform['sid']));
  foreach($result as $record) {
    if ($record->uid != 0) {
      $contact = redhen_engagement_rules_action_find_by_uid($record->uid);
      $nid = $record->nid;
    }
  };
  if ($contact) {
    redhen_enagement_rules_action_score_contact($score, $contact, 'Scored webform submission', 'node', $nid);
  }
}

/**
 * Helper function for finding a contact by an email address.
 */
function redhen_engagement_rules_action_find_by_mail($email) {
  $query = new EntityFieldQuery;
	$query
	  ->entityCondition('entity_type', 'redhen_contact')
    ->propertyCondition('redhen_state', REDHEN_STATE_ACTIVE)
    ->fieldCondition('redhen_contact_email', 'value', $email, '=');
	$result = $query->execute();
	if (!empty($result)) {
    $contact = redhen_contact_load(reset(array_keys($result['redhen_contact'])));
    return $contact;
  }
  else {
    return FALSE;
  }
}

/**
 * Helper function for finding a contact by an Drupal UID.
 */
function redhen_engagement_rules_action_find_by_uid($uid) {
  $user = user_load($uid);
  $contact = redhen_contact_load_by_user($user);
  if (!empty($contact)) {
    return $contact;
  }
  else {
    return FALSE;
  }
}

/**
 * Helper function for saving a comment engagement score when the Rule action is triggered.
 */
function redhen_enagement_rules_action_score_contact($score, $contact, $description = NULL, $entity_type = NULL, $entity_id = NULL) {
  // @todo: Should score be a single engagement score? Is that possible with Rules select list?
  foreach($score as $engagement_score) {
    // @todo: Reconsider the engagement score name pattern.
	  $engagement = redhen_engagement_create('engagement_score_' . $engagement_score, $contact->contact_id, $description, $entity_type, $entity_id);
    redhen_engagement_save($engagement);
	}
}