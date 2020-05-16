<?php

namespace Drupal\redhen_connection\Entity;

use Drupal\Core\Field\BaseFieldDefinition;
use Drupal\Core\Entity\ContentEntityBase;
use Drupal\Core\Entity\EntityChangedTrait;
use Drupal\Core\Entity\EntityInterface;
use Drupal\Core\Entity\EntityTypeInterface;
use Drupal\Core\StringTranslation\StringTranslationTrait;
use Drupal\Core\Url;
use Drupal\redhen_connection\ConnectionInterface;
use Drupal\redhen_contact\ContactInterface;
use Drupal\redhen_contact\Entity\Contact;

/**
 * Defines the Connection entity.
 *
 * @ingroup redhen_connection
 *
 * @ContentEntityType(
 *   id = "redhen_connection",
 *   label = @Translation("Connection"),
 *   label_singular = @Translation("Connection"),
 *   label_plural = @Translation("Connections"),
 *   label_count = @PluralTranslation(
 *     singular = "@count Connection",
 *     plural = "@count Connections",
 *   ),
 *   bundle_label = @Translation("Connection type"),
 *   handlers = {
 *     "view_builder" = "Drupal\Core\Entity\EntityViewBuilder",
 *     "list_builder" = "Drupal\redhen_connection\ConnectionListBuilder",
 *     "views_data" = "Drupal\redhen_connection\Entity\ConnectionViewsData",
 *
 *     "form" = {
 *       "default" = "Drupal\redhen_connection\Form\ConnectionForm",
 *       "add" = "Drupal\redhen_connection\Form\ConnectionForm",
 *       "edit" = "Drupal\redhen_connection\Form\ConnectionForm",
 *       "delete" = "Drupal\redhen_connection\Form\ConnectionDeleteForm",
 *     },
 *     "access" = "Drupal\redhen_connection\ConnectionAccessControlHandler",
 *     "route_provider" = {
 *       "html" = "Drupal\redhen_connection\ConnectionHtmlRouteProvider",
 *     },
 *   },
 *   base_table = "redhen_connection",
 *   revision_table = "redhen_connection_revision",
 *   admin_permission = "administer connection entities",
 *   entity_keys = {
 *     "id" = "id",
 *     "revision" = "revision_id",
 *     "bundle" = "type",
 *     "uuid" = "uuid",
 *     "uid" = "user_id",
 *     "langcode" = "langcode",
 *     "status" = "status",
 *   },
 *   links = {
 *     "canonical" = "/redhen/connection/{redhen_connection}",
 *     "add-form" = "/redhen/{redhen_type}/{entity}/connection/add/{redhen_connection_type}",
 *     "edit-form" = "/redhen/connection/{redhen_connection}/edit",
 *     "delete-form" = "/redhen/connection/{redhen_connection}/delete",
 *     "collection" = "/redhen/connection",
 *   },
 *   bundle_entity_type = "redhen_connection_type",
 *   field_ui_base_route = "entity.redhen_connection_type.edit_form"
 * )
 */
class Connection extends ContentEntityBase implements ConnectionInterface {
  use EntityChangedTrait;
  use StringTranslationTrait;

  /**
   * {@inheritdoc}
   */
  public function label() {
    $label_pattern = $this->type->entity->get('connection_label_pattern');
    return $this->t($label_pattern, array(
      '@label1' => $this->get('endpoint_1')->entity ? $this->get('endpoint_1')->entity->label() : "[entity 1 not found]",
      '@label2' => $this->get('endpoint_2')->entity ? $this->get('endpoint_2')->entity->label() : "[entity 2 not found]",
    ));
  }

  /**
   * {@inheritdoc}
   */
  public function getType() {
    return $this->bundle();
  }

  /**
   * {@inheritdoc}
   */
  public function getCreatedTime() {
    return $this->get('created')->value;
  }

  /**
   * {@inheritdoc}
   */
  public function setCreatedTime($timestamp) {
    $this->set('created', $timestamp);
    return $this;
  }

  /**
   * {@inheritdoc}
   *
   * This function, when inherited, crashes unexpectedly because there are some
   * unusual arguments in the paths for connections. We duplicate with a minor
   * tweak to the parameters.
   */
  public function toUrl($rel = 'canonical', array $options = []) {
    if ($this->id() === NULL) {
      throw new EntityMalformedException(sprintf('The "%s" entity cannot have a URI as it does not have an ID', $this->getEntityTypeId()));
    }

    // The links array might contain URI templates set in annotations.
    $link_templates = $this->linkTemplates();

    // Links pointing to the current revision point to the actual entity. So
    // instead of using the 'revision' link, use the 'canonical' link.
    if ($rel === 'revision' && $this instanceof RevisionableInterface && $this->isDefaultRevision()) {
      $rel = 'canonical';
    }

    if (isset($link_templates[$rel])) {
      $route_parameters = $this->urlRouteParameters($rel);
      $route_name = "entity.{$this->entityTypeId}." . str_replace(['-', 'drupal:'], ['_', ''], $rel);
      $uri = new Url($route_name, $route_parameters);
    }
    else {
      $bundle = $this->bundle();
      // A bundle-specific callback takes precedence over the generic one for
      // the entity type.
      $bundles = $this->entityManager()->getBundleInfo($this->getEntityTypeId());
      if (isset($bundles[$bundle]['uri_callback'])) {
        $uri_callback = $bundles[$bundle]['uri_callback'];
      }
      elseif ($entity_uri_callback = $this->getEntityType()->getUriCallback()) {
        $uri_callback = $entity_uri_callback;
      }

      // Invoke the callback to get the URI. If there is no callback, use the
      // default URI format.
      if (isset($uri_callback) && is_callable($uri_callback)) {
        $uri = call_user_func($uri_callback, $this);
      }
      else {
        throw new UndefinedLinkTemplateException("No link template '$rel' found for the '{$this->getEntityTypeId()}' entity type");
      }
    }

    // Pass the entity data through as options, so that alter functions do not
    // need to look up this entity again.
    $uri
      ->setOption('entity_type', $this->getEntityTypeId())
      ->setRouteParameter('redhen_type', $this->getFieldDefinitions()['endpoint_1']->getSetting('target_type'))
      ->setRouteParameter('entity', $this->id())
      ->setOption('entity', $this);

    // Here is our tweak, we include values for these route parameters that are
    // expected on the "add" form. The entity should, for accuracy, be set to
    // the endpoint 1 target.
    $uri
      ->setRouteParameter('redhen_type', $this->getFieldDefinitions()['endpoint_1']->getSetting('target_type'))
      ->setRouteParameter('entity', $this->id());

    // Display links by default based on the current language.
    // Link relations that do not require an existing entity should not be
    // affected by this entity's language, however.
    if (!in_array($rel, ['collection', 'add-page', 'add-form'], TRUE)) {
      $options += ['language' => $this->language()];
    }

    $uri_options = $uri->getOptions();
    $uri_options += $options;

    return $uri->setOptions($uri_options);
  }

  /**
   * {@inheritdoc}
   */
  public function isActive() {
    return (bool) $this->getEntityKey('status');
  }

  /**
   * {@inheritdoc}
   */
  public function setActive($active) {
    $this->set('status', $active ? REDHEN_CONNECTION_ACTIVE : REDHEN_CONNECTION_INACTIVE);
    return $this;
  }

  /**
   * {@inheritdoc}
   */
  public static function baseFieldDefinitions(EntityTypeInterface $entity_type) {
    $fields = parent::baseFieldDefinitions($entity_type);

    // Define base fields "endpoint_X" for each our endpoints.
    for ($x = 1; $x <= REDHEN_CONNECTION_ENDPOINTS; $x++) {
      // Set first endpoint to redhen_contact and second to redhen_org by default.
      $default_type = ($x & 1) ? 'redhen_contact' : 'redhen_org';

      $fields["endpoint_$x"] = BaseFieldDefinition::create('entity_reference')
        ->setLabel(t('Endpoint @x', array('@x' => $x)))
        ->setRequired(TRUE)
        ->setSetting('target_type', $default_type)
        ->setDisplayOptions('form', [
          'type' => 'entity_reference_autocomplete',
          'weight' => -1,
          'settings' => [
            'match_operator' => 'CONTAINS',
            'size' => '60',
            'placeholder' => '',
          ],
        ])
        ->setDisplayConfigurable('form', TRUE)
        ->setDisplayConfigurable('view', TRUE);
    }

    $fields['role'] = BaseFieldDefinition::create('entity_reference')
      ->setLabel(t('Role'))
      ->setRequired(TRUE)
      ->setRevisionable(TRUE)
      ->setSetting('target_type', 'redhen_connection_role')
      ->setSetting('handler', 'default')
      ->setDisplayOptions('form', [
        'type' => 'options_select',
        'weight' => -1,
      ])
      ->setDisplayConfigurable('form', TRUE)
      ->setDisplayConfigurable('view', TRUE);


    $fields['status'] = BaseFieldDefinition::create('boolean')
      ->setLabel(t('Active'))
      ->setDescription(t('A boolean indicating whether the connection is active.'))
      ->setDefaultValue(TRUE)
      ->setDisplayOptions('form', array(
        'type' => 'boolean_checkbox',
        'settings' => array(
          'display_label' => TRUE,
        ),
        'weight' => 16,
      ))
      ->setDisplayConfigurable('form', TRUE)
      ->setRevisionable(TRUE);

    $fields['created'] = BaseFieldDefinition::create('created')
      ->setLabel(t('Created'))
      ->setDescription(t('The time that the connection was created.'))
      ->setRevisionable(TRUE);

    $fields['changed'] = BaseFieldDefinition::create('changed')
      ->setLabel(t('Changed'))
      ->setDescription(t('The time that the connection was last edited.'))
      ->setRevisionable(TRUE);

    return $fields;
  }

  /**
   * {@inheritdoc}
   */
  public static function bundleFieldDefinitions(EntityTypeInterface $entity_type, $bundle, array $base_field_definitions) {
    /** @var \Drupal\redhen_connection\ConnectionTypeInterface $connection_type */
    $connection_type = ConnectionType::load($bundle);
    $fields = [];
    // Set bundle specific settings for each of our endpoint fields.
    for ($x = 1; $x <= REDHEN_CONNECTION_ENDPOINTS; $x++) {
      /** @var BaseFieldDefinition $fields[$field] */
      $endpoint_type = $connection_type->getEndpointEntityTypeId($x);
      $field = 'endpoint_' . $x;
      $fields[$field] = clone $base_field_definitions[$field];
      if ($endpoint_type) {
        $bundles = $connection_type->getEndpointBundles($x);
        $endpoint_entity = \Drupal::entityTypeManager()->getDefinition($endpoint_type);
        $label = (!empty($connection_type->getEndpointLabel($x))) ? $connection_type->getEndpointLabel($x) : $endpoint_entity->getLabel();
        $fields[$field]->setSetting('target_type', $endpoint_type)
          ->setLabel($label);
        if (!empty($connection_type->getEndpointDescription($x))) {
          $fields[$field]->setDescription($connection_type->getEndpointDescription($x));
        }
        if (!empty($bundles)) {
          $fields[$field]->setSetting('handler_settings', ['target_bundles' => $bundles]);
        }
      }
    }

    $fields['role'] = clone $base_field_definitions['role'];
    $fields['role']->setSetting('handler_settings', ['connection_type' => $connection_type->id()]);

    return $fields;
  }

  /**
   * {@inheritdoc}
   */
  public function hasRolePermission(EntityInterface $entity, $operation, ContactInterface $contact = NULL) {
    // @todo delete this method.
    if (!$contact) {
      $contact = Contact::loadByUser(\Drupal::currentUser());
    }
    if (!$contact) {
      return FALSE;
    }
    // Make sure we have a valid entity to check against.
    if (!($entity instanceof ConnectionInterface)) {
      $connection_type = ConnectionType::load($this->bundle());
      $endpoints = $connection_type->getEndpointFields($entity->getEntityTypeId());
      if (empty($endpoints)) {
        return FALSE;
      }
    }
    $role = $this->get('role')->entity;
    if (!$role) {
      return FALSE;
    }
    $permissions = $role->get('permissions');

    $entity_type = $entity->getEntityTypeId();
    // Determine which permission set to check:
    // $entity can be: connection, connected entity or secondary contact.
    $permission_set = 'entity';

    // Connection.
    if ($entity instanceof ConnectionInterface) {
      $permission_set = 'connection';
    }

    // Secondary contact.
    if ($entity_type == 'redhen_contact') {
      // @todo Additional connection check needed to ensure $entity is connected
      // indirectly to $account? Currently happening in ConnectionService:getIndirectConnections().
      $permission_set = 'contact';
    }

    return (is_array($permissions[$permission_set]) && in_array($operation, $permissions[$permission_set]));
  }
}
