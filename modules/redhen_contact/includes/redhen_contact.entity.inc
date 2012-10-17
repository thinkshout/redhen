<?php
/**
 * @file
 * Redhen Contact entity classses
 */

/**
 * The class used for contact entities.
 */
class RedhenContact extends Entity {

  public
    $first_name = '',
    $last_name = '',
    $contact_id = NULL,
    $uid = NULL,
    $author_uid = NULL,
    $redhen_state = REDHEN_STATE_ACTIVE,
    $type = '',
    $created = '',
    $updated = '';

  public function __construct(array $values = array()) {
    parent::__construct($values, 'redhen_contact');

    // ensure all field values are loaded
    // @todo: Figure out why we need this here.
    if ($this->contact_id) {
      field_attach_load('redhen_contact', array($this->contact_id => $this));
    }

    $this->email = $this->defaultEmail();
  }

  /**
   * Override buildContent() to add contact properties.
   */
  public function buildContent($view_mode = 'full', $langcode = NULL) {

    $wrapper = entity_metadata_wrapper('redhen_contact', $this);
    $user = $wrapper->user->value();
    
    $content['redhen_state'] = array(
      '#theme' => 'redhen_property_field',
      '#label' => t('State'),
      '#items' => array(
        array(
          '#markup' => ($this->redhen_state == REDHEN_STATE_ACTIVE) ? t('Active') : t('Inactive')
        ),
      ),
      '#classes' => 'field field-label-inline clearfix',
    );

    if ($user) {
      $user_uri = entity_uri('user', $user);
      $content['user'] = array(
        '#theme' => 'redhen_property_field',
        '#label' => t('Drupal User'),
        '#items' => array(
          array(
            '#markup' => l($user->name, $user_uri['path'])
          ),
        ),
        '#classes' => 'field field-label-inline clearfix',
      );
    }

    return entity_get_controller($this->entityType)
      ->buildContent($this, $view_mode, $langcode, $content);
  }

  /**
   * Specifies the default label, which is picked up by label() by default.
   */
  protected function defaultLabel() {
    $type = redhen_contact_get_types($this->type);
    $wrapper = entity_metadata_wrapper('redhen_contact', $this);
    $name = $wrapper->full_name->value();
    return $name . ' (' . $type->label . ')';
  }

  /**
   * Specifify URI
   */
  protected function defaultUri() {
    return array('path' => 'redhen/contact/' . $this->identifier());
  }

  /**
   * Return default email from email field.
   *
   * @return string
   */
  public function defaultEmail() {
    if (!empty($this->{REDHEN_CONTACT_EMAIL_FIELD})) {
      foreach ($this->{REDHEN_CONTACT_EMAIL_FIELD}[LANGUAGE_NONE] as $email) {
        if ($email['default']) {
          return $email['value'];
        }
      }
    }
    return '';
  }

  /**
   * Return all emails and their properties in an array..
   *
   * @return array
   */
  public function allEmail() {
    $emails = array();
    if (!empty($this->{REDHEN_CONTACT_EMAIL_FIELD})) {
      $emails = $this->{REDHEN_CONTACT_EMAIL_FIELD}[LANGUAGE_NONE];
    }
    return $emails;
  }

  /**
   * Determine if contact has a given email.
   *
   * @param $email
   *
   * @return bool
   */
  public function hasEmail($email) {
    if (!empty($this->{REDHEN_CONTACT_EMAIL_FIELD})) {
      foreach ($this->{REDHEN_CONTACT_EMAIL_FIELD}[LANGUAGE_NONE] as $contact_email) {
        if ($contact_email['value'] === $email) {
          return TRUE;
        }
      }
    }
    return FALSE;
  }

  /**
   * Sets an email for a contact.
   *
   * @param $email
   * @param int $default
   * @param int $bulk
   * @param int $hold
   * @param int $label_id
   */
  public function setEmail($email, $default = 1, $bulk = 1, $hold = 0, $label_id = 0) {
    $this->{REDHEN_CONTACT_EMAIL_FIELD}[LANGUAGE_NONE][] = array(
      'value' => $email,
      'default' => $default,
      'bulk' => $bulk,
      'hold' => $hold,
      'label_id' => $label_id
    );
  }

  public function setUser() {
    return entity_get_controller($this->entityType)->setUser($this);
  }

  public function deleteUser($delete) {
    return entity_get_controller($this->entityType)->deleteUser($this, $delete);
  }

  /**
   * Set the redhen_state for a contact
   *
   * @param $state
   *  The REDHEN_STATE_* value to use
   */
  public function setState($state) {
    // allow other modules to act on a state change
    module_invoke_all('redhen_contact_set_state', $this->redhen_state, $state);
    $this->redhen_state = $state;
    return entity_get_controller($this->entityType)->save($this);
  }

}


/**
 * Controls metadata for Redhen contacts
 *
 * Redhen utilizes custom property attributes to determine if a property
 * should be available as a filter on the listing page. These attributes are
 *   filter: TRUE to add as a filter.
 *   filter_operator: EFQ supported operators. Defaults to = or IN depending on value submitted
 *   field_type: textfield, select, etc.
 *   options list: callback that returns the options for this field.
 */
class RedhenContactMetadataController extends EntityDefaultMetadataController {

  public function entityPropertyInfo() {
    $info = parent::entityPropertyInfo();
    $properties = &$info[$this->type]['properties'];

    $properties['created'] = array(
      'label' => t("Created"),
      'description' => t("The date the contact was created."),
      'type' => 'date',
      'schema field' => 'created',
      'setter callback' => 'entity_property_verbatim_set',
    );
    $properties['updated'] = array(
      'label' => t("Updated"),
      'description' => t("The date the contact was updated."),
      'type' => 'date',
      'schema field' => 'updated',
      'setter callback' => 'entity_property_verbatim_set',
    );

    $properties['first_name'] = array_merge($properties['first_name'], array(
      'label' => t('First Name'),
      'filter' => TRUE,
      'field_type' => 'textfield',
      'setter callback' => 'entity_property_verbatim_set',
    ));
    $properties['last_name'] = array_merge($properties['last_name'], array(
      'label' => t('Last Name'),
      'filter' => TRUE,
      'field_type' => 'textfield',
      'setter callback' => 'entity_property_verbatim_set',
    ));

    $properties['full_name'] = array(
      'label' => t('Full name'),
      'description' => t('The full name of the contact.'),
      'filter' => TRUE,
      'type' => 'text',
      'getter callback' => 'redhen_contact_property_full_name_get',
      'computed' => TRUE,
      'entity views field' => TRUE,
    );

    $properties['redhen_state'] = array_merge($properties['redhen_state'], array(
      'label' => t('State'),
      'filter' => TRUE,
      'field_type' => 'select',
      'options list' => 'redhen_state_options',
      'setter callback' => 'entity_property_verbatim_set',
    ));

    // Entities
    $properties['user'] = array(
      'label' => t("User"),
      'type' => 'user',
      'description' => t("The Drupal user associated with the contact."),
      'getter callback' => 'redhen_contact_property_user_get',
    );
    $properties['author'] = array(
      'label' => t("Author"),
      'type' => 'user',
      'description' => t("The author of the contact record."),
      'schema field' => 'author_uid'
    );

    return $info;
  }

}

/**
 * RedhenContact Views Controller class.
 */
class RedhenContactViewsController extends EntityDefaultViewsController {

  /**
   * Add extra fields to views_data().
   */
  public function views_data() {
    $data = parent::views_data();

    // Base table definition for redhen_contact_user so we can use it for joins
    $data['redhen_contact_user']['table']['group'] = t('User');

    $data['redhen_contact_user']['table']['join'] = array(
      'redhen_contact' => array(
        'left_field' => 'contact_id',
        'field' => 'contact_id',
        // Only join where the linkage is active
        'extra' => array(array('field' => 'status', 'value' => '1', 'operator' => '=')),
      ),
      'users' => array(
        'left_field' => 'uid',
        'field' => 'uid',
        // Only join where the linkage is active
        'extra' => array(array('field' => 'status', 'value' => '1', 'operator' => '=')),
      ),
    );

    // Add user relationship to contacts
    $data['redhen_contact']['table']['join'] = array(
      'users' => array(
        'left_table' => 'redhen_contact_user',
        'left_field' => 'contact_id',
        'field' => 'contact_id',

      ),
      'redhen_contact_user' => array(
        'left_field' => 'contact_id',
        'field' => 'contact_id',
      ),
    );

    $data['users']['table']['join'] = array(
      'redhen_contact' => array(
        'left_table' => 'redhen_contact_user',
        'left_field' => 'uid',
        'field' => 'uid',
      ),
      'redhen_contact_user' => array(
        'left_field' => 'uid',
        'field' => 'uid',
      ),
    );

    return $data;
  }
}

/*
 * Callback to get $contact->full_name
 */
function redhen_contact_property_full_name_get(RedhenContact $contact, array $options, $property_name, $entity_type) {
  $name = $contact->first_name . ' ' . $contact->last_name;
  $context = array(
    'contact' => clone $contact,
  );
  drupal_alter('redhen_contact_name', $name, $context);
  return $name;
}

/**
 * Callback to get $contact->user.
 */
function redhen_contact_property_user_get(RedhenContact $contact, array $options, $property_name, $entity_type) {
  if (is_numeric($contact->uid)) {
    return entity_load_single('user', $contact->uid);
  }
}
